package main

import (
	"bytes"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"time"
)

var (
	seedDate        = flag.String("seed_date", "", "Skip transactions before this date (YYYY-MM-DD); empty = no filter")
	plaidConfigPath = flag.String("plaid_config", "plaid-config.json", "Path to Plaid config file")
	plaidBaseURL    = flag.String("plaid_base_url", "https://production.plaid.com", "Plaid base URL")
	vigilBaseURL    = flag.String("vigil_base_url", "http://localhost:8080", "Vigil server base URL")
)

type JSONFloat64 float64

type PlaidItem struct {
	ItemID      string `json:"item_id"`
	AccessToken string `json:"access_token"`
	SyncCursor  string `json:"sync_cursor"`
	Alias       string `json:"alias"`
	Accounts    map[string]*PlaidAccount
}

type PlaidContext struct {
	ClientID string
	Secret   string
	Items    []PlaidItem
}

type PlaidAccount struct {
	AccountID string `json:"account_id"`
	Balances  struct {
		Current float64 `json:"current"`
	} `json:"balances"`
	Name         string `json:"name"`
	Type         string `json:"subtype"`
	Transactions []*PlaidTransaction
}

type PlaidTransaction struct {
	TransactionID string      `json:"transaction_id"`
	AccountID     string      `json:"account_id"`
	Amount        JSONFloat64 `json:"amount"`
	Date          string      `json:"date"`
	Name          string      `json:"name"`
	Pending       bool        `json:"pending"`
}

type VigilTransaction struct {
	ExternalID           string      `json:"external_id"`
	Type                 string      `json:"type"`
	Amount               JSONFloat64 `json:"amount"`
	Merchant             string      `json:"merchant"`
	TransactionTimestamp int64       `json:"transaction_timestamp"`
}

type VigilAccount struct {
	Name         string             `json:"name"`
	Type         string             `json:"type"`
	Transactions []VigilTransaction `json:"transactions,omitempty"`
}

func main() {
	flag.Parse()
	log.Println("conduit: starting")

	ctx, err := loadPlaidContext(*plaidConfigPath)
	if err != nil {
		log.Fatalf("loading plaid-cli config: %v", err)
	}
	log.Printf("conduit: loaded config (env=%s, items=%d)", ctx.Items[0].Alias, len(ctx.Items))

	log.Println("conduit: fetching plaid accounts")
	if err := GetAccounts(ctx); err != nil {
		log.Fatalf("getting accounts: %v", err)
	}

	log.Println("conduit: running incremental sync")
	if err := SyncTransactions(ctx); err != nil {
		log.Fatalf("syncing transactions: %v", err)
	}

	log.Println("conduit: transforming to vigil accounts")
	vigilAccounts, err := ToVigilAccounts(ctx)
	if err != nil {
		log.Fatalf("transforming accounts: %v", err)
	}
	log.Printf("conduit: transformed %d vigil accounts", len(vigilAccounts))

	log.Println("conduit: syncing to vigil")
	if err := SyncToVigil(vigilAccounts); err != nil {
		log.Fatalf("syncing to vigil: %v", err)
	}

	log.Println("conduit: done")
}

func SyncToVigil(accounts map[string]*VigilAccount) error {
	for _, acc := range accounts {
		log.Printf("SyncToVigil: processing account %q", acc.Name)

		if err := EnsureVigilAccount(acc); err != nil {
			return fmt.Errorf("ensure account %q: %w", acc.Name, err)
		}

		imported, err := ImportVigilTransactions(acc)
		if err != nil {
			return fmt.Errorf("import transactions for %q: %w", acc.Name, err)
		}

		log.Printf("SyncToVigil: imported %d transactions into %q", imported, acc.Name)
	}

	return nil
}

func EnsureVigilAccount(acc *VigilAccount) error {
	log.Printf("EnsureVigilAccount: checking if %q exists", acc.Name)

	resp, err := http.Get(fmt.Sprintf("%s/accounts/%s", *vigilBaseURL, acc.Name))
	if err != nil {
		return fmt.Errorf("get account: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode == 200 {
		log.Printf("EnsureVigilAccount: %q already exists", acc.Name)
		return nil
	}

	if resp.StatusCode != 404 {
		return fmt.Errorf("get account returned unexpected status %d", resp.StatusCode)
	}

	log.Printf("EnsureVigilAccount: creating %q (type=%s)", acc.Name, acc.Type)

	body, err := json.Marshal(map[string]string{
		"name": acc.Name,
		"type": acc.Type,
	})
	if err != nil {
		return fmt.Errorf("marshal create account request: %w", err)
	}

	createResp, err := http.Post(
		fmt.Sprintf("%s/accounts", *vigilBaseURL),
		"application/json",
		bytes.NewReader(body),
	)
	if err != nil {
		return fmt.Errorf("create account: %w", err)
	}
	defer createResp.Body.Close()

	if createResp.StatusCode != 201 {
		return fmt.Errorf("create account returned unexpected status %d", createResp.StatusCode)
	}

	log.Printf("EnsureVigilAccount: created %q (%s)", acc.Name, acc.Type)
	return nil
}

func ImportVigilTransactions(acc *VigilAccount) (int, error) {
	if len(acc.Transactions) == 0 {
		log.Printf("ImportVigilTransactions: no transactions for %q, skipping", acc.Name)
		return 0, nil
	}

	log.Printf("ImportVigilTransactions: importing %d transactions into %q", len(acc.Transactions), acc.Name)

	body, err := json.Marshal(acc.Transactions)
	if err != nil {
		return 0, fmt.Errorf("marshal transactions: %w", err)
	}

	resp, err := http.Post(
		fmt.Sprintf("%s/accounts/%s/transactions/import", *vigilBaseURL, acc.Name),
		"application/json",
		bytes.NewReader(body),
	)
	if err != nil {
		return 0, fmt.Errorf("import transactions: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != 200 {
		return 0, fmt.Errorf("import transactions returned unexpected status %d", resp.StatusCode)
	}

	var result map[string]int
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		return 0, fmt.Errorf("decode import response: %w", err)
	}

	log.Printf("ImportVigilTransactions: server reported %d newly imported for %q", result["imported"], acc.Name)
	return result["imported"], nil
}

func GetAccounts(ctx *PlaidContext) error {
	type GetAccountRequest struct {
		ClientID    string `json:"client_id"`
		Secret      string `json:"secret"`
		AccessToken string `json:"access_token"`
	}

	type GetAccountResponse struct {
		Accounts []*PlaidAccount `json:"accounts"`
	}

	for i := range ctx.Items {
		item := &ctx.Items[i]
		log.Printf("GetAccounts: fetching accounts for item %q", item.Alias)

		resp, err := Call[GetAccountRequest, GetAccountResponse](
			"/accounts/get",
			GetAccountRequest{
				ClientID:    ctx.ClientID,
				Secret:      ctx.Secret,
				AccessToken: item.AccessToken,
			})
		if err != nil {
			return err
		}

		item.Accounts = make(map[string]*PlaidAccount)
		for _, account := range resp.Accounts {
			item.Accounts[account.AccountID] = account
		}

		log.Printf("GetAccounts: item %q has %d accounts", item.Alias, len(item.Accounts))
	}

	return nil
}

func SyncTransactions(ctx *PlaidContext) error {
	type SyncTransactionsRequest struct {
		ClientID    string `json:"client_id"`
		Secret      string `json:"secret"`
		AccessToken string `json:"access_token"`
		Cursor      string `json:"cursor,omitempty"`
	}

	type SyncTransactionsResponse struct {
		Added   []PlaidTransaction `json:"added"`
		Cursor  string             `json:"next_cursor"`
		HasMore bool               `json:"has_more"`
	}

	for i := range ctx.Items {
		item := &ctx.Items[i]
		log.Printf("SyncTransactions: syncing item %q (cursor=%q)", item.Alias, item.SyncCursor)

		cursor := item.SyncCursor
		page := 0
		for {
			resp, err := Call[SyncTransactionsRequest, SyncTransactionsResponse](
				"/transactions/sync",
				SyncTransactionsRequest{
					ClientID:    ctx.ClientID,
					Secret:      ctx.Secret,
					AccessToken: item.AccessToken,
					Cursor:      cursor,
				})
			if err != nil {
				return err
			}

			log.Printf("SyncTransactions: page %d -- %d added for item %q", page, len(resp.Added), item.Alias)

			for _, t := range resp.Added {
				if t.Pending {
					continue
				}

				account, ok := item.Accounts[t.AccountID]
				if !ok {
					log.Printf("SyncTransactions: unknown account_id %q, skipping transaction %q", t.AccountID, t.TransactionID)
					continue
				}

				t := t
				account.Transactions = append(account.Transactions, &t)
			}

			cursor = resp.Cursor
			if !resp.HasMore {
				break
			}
			page++
		}

		item.SyncCursor = cursor
		log.Printf("SyncTransactions: item %q done (new cursor=%q)", item.Alias, item.SyncCursor)
	}

	return nil
}

func ToVigilAccounts(ctx *PlaidContext) (map[string]*VigilAccount, error) {
	accounts := make(map[string]*VigilAccount)
	prefixes := map[string]string{
		"capital_one": "Capital One ",
		"schwab":      "Schwab ",
		"sofi":        "",
	}
	accountTypes := map[string]string{
		"checking":    "CHECKING",
		"savings":     "SAVINGS",
		"brokerage":   "BROKERAGE",
		"stock plan":  "BROKERAGE",
		"credit card": "CREDIT_CARD",
	}

	var cutoff time.Time
	if *seedDate != "" {
		var err error
		cutoff, err = time.Parse("2006-01-02", *seedDate)
		if err != nil {
			return nil, fmt.Errorf("parse seed date: %w", err)
		}
	}

	for _, item := range ctx.Items {
		prefix, ok := prefixes[item.Alias]
		if !ok {
			log.Printf("ToVigilAccounts: no prefix mapped for plaid item %q, skipping", item.Alias)
			continue
		}

		for _, plaidAccount := range item.Accounts {
			vigilName := prefix + plaidAccount.Name
			accountType, ok := accountTypes[plaidAccount.Type]
			if !ok {
				log.Printf("ToVigilAccounts: no account type mapped for plaid account %q, skipping", vigilName)
				continue
			}

			acc, ok := accounts[vigilName]
			if !ok {
				acc = &VigilAccount{Name: vigilName, Type: accountType}
				accounts[vigilName] = acc
			}

			for _, t := range plaidAccount.Transactions {
				transactionType := "WITHDRAWAL"
				amount := t.Amount
				if amount < 0 {
					transactionType = "DEPOSIT"
					amount = -amount
				}

				date, err := time.Parse("2006-01-02", t.Date)
				if err != nil {
					log.Printf("ToVigilAccounts: parse date %q for transaction %q: %v", t.Date, t.TransactionID, err)
					continue
				}

				if !cutoff.IsZero() && date.Before(cutoff) {
					continue
				}

				acc.Transactions = append(acc.Transactions, VigilTransaction{
					ExternalID:           t.TransactionID,
					Type:                 transactionType,
					Amount:               amount,
					Merchant:             t.Name,
					TransactionTimestamp: date.UTC().Unix(),
				})
			}

			log.Printf("ToVigilAccounts: mapped %q -> %q (%d transactions)", plaidAccount.Name, vigilName, len(acc.Transactions))
		}
	}

	return accounts, nil
}

func loadPlaidContext(path string) (*PlaidContext, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read config: %w", err)
	}

	var cfg struct {
		ClientID     string `json:"client_id"`
		Env          string `json:"env"`
		Environments map[string]struct {
			Secret string      `json:"secret"`
			Items  []PlaidItem `json:"items"`
		} `json:"environments"`
	}

	if err := json.Unmarshal(data, &cfg); err != nil {
		return nil, fmt.Errorf("unmarshal config: %w", err)
	}

	env, ok := cfg.Environments[cfg.Env]
	if !ok {
		return nil, fmt.Errorf("environment %q not found in config", cfg.Env)
	}

	for i := range env.Items {
		env.Items[i].SyncCursor = ""
	}

	return &PlaidContext{
		ClientID: cfg.ClientID,
		Secret:   env.Secret,
		Items:    env.Items,
	}, nil
}

func (f JSONFloat64) MarshalJSON() ([]byte, error) {
	return fmt.Appendf(nil, "%f", float64(f)), nil
}

func Call[T, U any](endpoint string, req T) (*U, error) {
	request, err := json.Marshal(req)
	if err != nil {
		return nil, err
	}

	resp, err := http.Post(fmt.Sprintf("%s%s", *plaidBaseURL, endpoint), "application/json", bytes.NewReader(request))
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		body, _ := io.ReadAll(resp.Body)
		return nil, fmt.Errorf("plaid %s returned status %d: %s", endpoint, resp.StatusCode, body)
	}

	var result U
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		return nil, err
	}

	return &result, nil
}
