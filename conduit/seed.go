package main

import (
	"encoding/csv"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"time"
)

func SeedVigil(vigilEpoch time.Time, vigilSeedFile string) error {
	if vigilSeedFile == "" {
		log.Printf("SeedVigil: no seed file, skipping seed")
		return nil
	}

	log.Printf("SeedVigil: reading %q", vigilSeedFile)

	accounts, err := ReadSeedFile(vigilSeedFile, vigilEpoch.UTC().Unix())
	if err != nil {
		return fmt.Errorf("read seed file: %w", err)
	}

	for name, transactions := range accounts {
		log.Printf("SeedVigil: importing %d seed transaction(s) into %q", len(transactions), name)

		imported, err := ImportVigilTransactions(&VigilAccount{
			Name:         name,
			Transactions: transactions,
		})
		if err != nil {
			return fmt.Errorf("seed account %q: %w", name, err)
		}

		if imported == 0 {
			log.Printf("SeedVigil: no transactions for %q, skipping", name)
		} else {
			log.Printf("SeedVigil: imported %d transactions into %q", imported, name)
		}
	}

	return nil
}

// TODO(clean up)
func ReadSeedFile(path string, vigilEpoch int64) (map[string][]VigilTransaction, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("open: %w", err)
	}
	defer f.Close()

	reader := csv.NewReader(f)
	reader.FieldsPerRecord = 5

	if _, err := reader.Read(); err == io.EOF {
		return nil, nil
	}

	accounts := make(map[string][]VigilTransaction)
	lineNum := 0
	for {
		lineNum++
		record, err := reader.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			return nil, fmt.Errorf("line %d: %w", lineNum, err)
		}

		accountName := record[0]
		transactionType := record[2]

		if transactionType != "DEPOSIT" && transactionType != "WITHDRAWAL" {
			return nil, fmt.Errorf("line %d: invalid type %q (want DEPOSIT or WITHDRAWAL)", lineNum, transactionType)
		}

		amount, err := strconv.ParseFloat(record[3], 64)
		if err != nil {
			return nil, fmt.Errorf("line %d: invalid amount %f: %w", lineNum, amount, err)
		}

		accounts[accountName] = append(accounts[accountName], VigilTransaction{
			ExternalID:           record[1],
			Type:                 transactionType,
			Amount:               JSONFloat64(amount),
			Merchant:             record[4],
			TransactionTimestamp: vigilEpoch,
		})
	}

	return accounts, nil
}
