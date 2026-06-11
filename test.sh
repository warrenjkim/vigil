#!/usr/bin/env bash

set -e

BASE="http://localhost:8080"
DB="vigil_test.db"
PASS=0
FAIL=0

RED='\033[38;2;204;36;29m'
GREEN='\033[38;2;152;151;26m'
YELLOW='\033[38;2;215;153;33m'
BOLD='\033[1m'
RESET='\033[0m'

cleanup() {
  kill $SERVER_PID 2>/dev/null || true
  rm -f $DB $DB-shm $DB-wal
}
trap cleanup EXIT

section() {
  echo -e "\n${BOLD}$1${RESET}"
}

assert_status() {
  local description=$1
  local expected=$2
  local actual=$3
  if [ "$actual" = "$expected" ]; then
    echo -e "${GREEN}PASS${RESET}: $description"
    ((PASS++)) || true
  else
    echo -e "${RED}FAIL${RESET}: $description (expected=${YELLOW}$expected${RESET}, actual=${YELLOW}$actual${RESET})"
    ((FAIL++)) || true
  fi
}

assert_contains() {
  local description=$1
  local expected=$2
  local actual=$3
  if echo "$actual" | grep -q "$expected"; then
    echo -e "${GREEN}PASS${RESET}: $description"
    ((PASS++)) || true
  else
    echo -e "${RED}FAIL${RESET}: $description (expected to contain=${YELLOW}'$expected'${RESET}, actual=${YELLOW}'$actual'${RESET})"
    ((FAIL++)) || true
  fi
}

echo -e "${BOLD}===== building with --db=${DB} =====${RESET}"
bazel build //vigil:vigil 2>/dev/null

rm -f $DB $DB-shm $DB-wal
./bazel-bin/vigil/vigil --db=$DB >/dev/null 2>&1 &
SERVER_PID=$!
disown $SERVER_PID
sleep 1

if ! curl -s -o /dev/null $BASE/health; then
  echo -e "${RED}ERROR: server failed to start${RESET}"
  exit 1
fi

section "===== accounts ====="

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts \
  -H "Content-Type: application/json" \
  -d '{"name": "brokerage", "type": "BROKERAGE"}')
assert_status "Accounts/CreateAccount.ValidRequest/Returns201" "201" "$code"

body=$(curl -s $BASE/accounts)
assert_contains "Accounts/ListAccounts.AfterCreate/ContainsAccount" '"name":"brokerage"' "$body"

body=$(curl -s $BASE/accounts/brokerage)
assert_contains "Accounts/GetAccount.ExistingAccount/ReturnsName" '"name": "brokerage"' "$body"
assert_contains "Accounts/GetAccount.ExistingAccount/ReturnsType" '"type": "BROKERAGE"' "$body"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts \
  -H "Content-Type: application/json" \
  -d '{"name": "brokerage", "type": "BROKERAGE"}')
assert_status "Accounts/CreateAccount.DuplicateName/Returns500" "500" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" $BASE/accounts/nonexistent)
assert_status "Accounts/GetAccount.NonexistentAccount/Returns404" "404" "$code"

section "===== transactions ====="

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/transactions \
  -H "Content-Type: application/json" \
  -d '{"type": "DEPOSIT", "amount": 10000.0, "description": "initial deposit"}')
assert_status "Transactions/CreateTransaction.ValidDeposit/Returns201" "201" "$code"

body=$(curl -s $BASE/accounts/brokerage/transactions)
assert_contains "Transactions/ListTransactions.AfterDeposit/ContainsType" '"type":"DEPOSIT"' "$body"
assert_contains "Transactions/ListTransactions.AfterDeposit/ContainsAmount" '"amount":10000' "$body"
assert_contains "Transactions/ListTransactions.AfterDeposit/ContainsDescription" '"description":"initial deposit"' "$body"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/transactions \
  -H "Content-Type: application/json" \
  -d '{"type": "WITHDRAWAL", "amount": 500.0}')
assert_status "Transactions/CreateTransaction.ValidWithdrawal/Returns201" "201" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/nonexistent/transactions \
  -H "Content-Type: application/json" \
  -d '{"type": "DEPOSIT", "amount": 100.0}')
assert_status "Transactions/CreateTransaction.NonexistentAccount/Returns500" "500" "$code"

section "===== trades and holdings ====="

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "BUY", "ticker": "GOOG", "shares": 10.0, "price": 150.0, "description": "initial position"}')
assert_status "Trades/CreateTrade.BuyNewTicker/Returns201" "201" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "BUY", "ticker": "AAPL", "shares": 5.0, "price": 200.0}')
assert_status "Trades/CreateTrade.BuySecondTicker/Returns201" "201" "$code"

body=$(curl -s $BASE/accounts/brokerage/holdings)
assert_contains "Holdings/ListHoldings.AfterBuys/ContainsGOOG" '"ticker":"GOOG"' "$body"
assert_contains "Holdings/ListHoldings.AfterBuys/ContainsAAPL" '"ticker":"AAPL"' "$body"
assert_contains "Holdings/ListHoldings.AfterBuys/GOOGSharesCorrect" '"shares":10' "$body"
assert_contains "Holdings/ListHoldings.AfterBuys/AAPLSharesCorrect" '"shares":5' "$body"

body=$(curl -s $BASE/accounts/brokerage/holdings/GOOG)
assert_contains "Holdings/GetHolding.ExistingHolding/ReturnsTicker" '"ticker":"GOOG"' "$body"
assert_contains "Holdings/GetHolding.ExistingHolding/ReturnsShares" '"shares":10' "$body"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "BUY", "ticker": "GOOG", "shares": 5.0, "price": 155.0}')
assert_status "Trades/CreateTrade.BuyExistingTicker/Returns201" "201" "$code"

body=$(curl -s $BASE/accounts/brokerage/holdings/GOOG)
assert_contains "Holdings/GetHolding.AfterSecondBuy/SharesAccumulate" '"shares":15' "$body"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "SELL", "ticker": "GOOG", "shares": 3.0, "price": 160.0, "description": "partial sell"}')
assert_status "Trades/CreateTrade.PartialSell/Returns201" "201" "$code"

body=$(curl -s $BASE/accounts/brokerage/holdings/GOOG)
assert_contains "Holdings/GetHolding.AfterPartialSell/SharesReduced" '"shares":12' "$body"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "SELL", "ticker": "GOOG", "shares": 12.0, "price": 165.0, "description": "close position"}')
assert_status "Trades/CreateTrade.SellAll/Returns201" "201" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" $BASE/accounts/brokerage/holdings/GOOG)
assert_status "Holdings/GetHolding.AfterFullSell/Returns404" "404" "$code"

body=$(curl -s $BASE/accounts/brokerage/holdings)
assert_contains "Holdings/ListHoldings.AfterFullSellGOOG/OnlyAAPLRemains" '"ticker":"AAPL"' "$body"

body=$(curl -s $BASE/accounts/brokerage/trades)
assert_contains "Trades/ListTrades.FullHistory/ContainsBuy" '"type":"BUY"' "$body"
assert_contains "Trades/ListTrades.FullHistory/ContainsSell" '"type":"SELL"' "$body"
assert_contains "Trades/ListTrades.FullHistory/ContainsAAPL" '"ticker":"AAPL"' "$body"

section "===== error cases ====="

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "SELL", "ticker": "GOOG", "shares": 1.0, "price": 160.0}')
assert_status "Trades/CreateTrade.SellWithNoPosition/Returns422" "422" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "SELL", "ticker": "AAPL", "shares": 100.0, "price": 200.0}')
assert_status "Trades/CreateTrade.SellMoreThanOwned/Returns422" "422" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" $BASE/accounts/brokerage/holdings/GOOG)
assert_status "Holdings/GetHolding.NonexistentTicker/Returns404" "404" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" -X POST $BASE/accounts/brokerage/trades \
  -H "Content-Type: application/json" \
  -d '{"type": "bad_type", "ticker": "GOOG", "shares": 1.0, "price": 160.0}')
assert_status "Trades/CreateTrade.UnrecognizedType/Returns400" "400" "$code"

section "===== cascade delete ====="

code=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE $BASE/accounts/brokerage)
assert_status "Accounts/DeleteAccount.ExistingAccount/Returns204" "204" "$code"

code=$(curl -s -o /dev/null -w "%{http_code}" $BASE/accounts/brokerage)
assert_status "Accounts/GetAccount.AfterDelete/Returns404" "404" "$code"

body=$(curl -s $BASE/accounts)
assert_contains "Accounts/ListAccounts.AfterDelete/ReturnsEmpty" "\[\]" "$body"

echo ""
echo -e "${BOLD}================================${RESET}"
echo -e "${GREEN}PASSED${RESET}: $PASS"
echo -e "${RED}FAILED${RESET}: $FAIL"
echo -e "${BOLD}================================${RESET}"

cleanup

if [ $FAIL -gt 0 ]; then
  exit 1
fi
