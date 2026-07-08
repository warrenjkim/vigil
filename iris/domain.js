const CASH_TYPES = new Set(["CHECKING", "SAVINGS", "CREDIT_CARD"]);
const BROKERAGE_TYPES = new Set([
  "BROKERAGE",
  "401K",
  "ROTH_IRA",
  "TRADITIONAL_IRA",
]);

const PILL_CLASS = {
  BUY: "buy",
  SELL: "sell",
  DEPOSIT: "deposit",
  WITHDRAWAL: "withdrawal",
};

function costBasisMap(trades) {
  const basis = {};
  trades.forEach((t) => {
    const sign = t.type === "BUY" ? 1 : -1;
    basis[t.ticker] = (basis[t.ticker] || 0) + sign * t.shares * t.price;
  });

  return basis;
}
