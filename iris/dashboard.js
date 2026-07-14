import {
  CASH_TYPES,
  BROKERAGE_TYPES,
  PILL_CLASS,
  costBasisMap,
} from "/static/domain.js";
import { formatPrice, formatShares, formatDate } from "/static/format.js";
import { fetchJson } from "/static/api.js";

const ACTIVITY_LIMIT = 10;

function renderSummary(accountData) {
  let totalCash = 0;
  let totalPortfolio = 0;

  accountData.forEach(({ account, detail, basis }) => {
    if (CASH_TYPES.has(account.type) && detail.balance != null) {
      totalCash += detail.balance;
    }
    if (BROKERAGE_TYPES.has(account.type)) {
      totalPortfolio += Object.values(basis).reduce((s, v) => s + v, 0);
    }
  });

  document.getElementById("summary").innerHTML = `
          <div class="metric">
            <div class="label">total cash</div>
            <div class="value cash">${formatPrice(totalCash)}</div>
          </div>
          <div class="metric">
            <div class="label">portfolio cost basis</div>
            <div class="value portfolio">${formatPrice(totalPortfolio)}</div>
          </div>
          <div class="metric">
            <div class="label">net worth (est.)</div>
            <div class="value">${formatPrice(totalCash + totalPortfolio)}</div>
          </div>
        `;
}

function renderAccounts(accountData) {
  const grid = document.getElementById("accounts-grid");
  if (!accountData.length) {
    grid.innerHTML = '<div class="empty">no accounts</div>';
    return;
  }

  grid.innerHTML = accountData
    .map(({ account, detail, holdings, basis }) => {
      const href = `/accounts/detail?name=${encodeURIComponent(account.name)}`;

      if (CASH_TYPES.has(account.type)) {
        return `
                <a class="account-card" href="${href}">
                  <div class="account-name">${account.name}</div>
                  <div class="account-type">${account.type}</div>
                  <div class="account-value">${detail.balance != null ? formatPrice(detail.balance) : "—"}</div>
                </a>
              `;
      }

      return `
              <a class="account-card" href="${href}">
                <div class="account-name">${account.name}</div>
                <div class="account-type">${account.type}</div>
                <div class="account-value">${formatPrice(holdings.reduce((s, h) => s + (basis[h.ticker] || 0), 0))}</div>
                <div class="account-sub">cost basis · ${holdings.length} position${holdings.length !== 1 ? "s" : ""}</div>
              </a>
            `;
    })
    .join("");
}

function renderHoldings(accountData) {
  const container = document.getElementById("holdings-container");
  const rows = [];
  accountData.forEach(({ account, holdings, basis }) => {
    holdings.forEach((h) => {
      rows.push({
        account: account.name,
        ticker: h.ticker,
        shares: h.shares,
        basis: basis[h.ticker] || 0,
      });
    });
  });

  if (!rows.length) {
    container.innerHTML = '<div class="empty">no holdings</div>';
    return;
  }

  container.innerHTML = `
          <table>
            <thead>
              <tr>
                <th>ticker</th>
                <th>account</th>
                <th class="right">shares</th>
                <th class="right">cost basis</th>
              </tr>
            </thead>
            <tbody>
              ${rows
                .map(
                  (h) => `
                <tr>
                  <td class="label-col">${h.ticker}</td>
                  <td class="label-col">${h.account}</td>
                  <td class="right">${formatShares(h.shares)}</td>
                  <td class="right">${formatPrice(h.basis)}</td>
                </tr>`,
                )
                .join("")}
            </tbody>
          </table>
        `;
}

function renderActivity(accountData) {
  const container = document.getElementById("activity-container");
  const events = [];

  accountData.forEach(({ account, transactions, trades }) => {
    transactions.forEach((t) => {
      events.push({
        id: t.id,
        account: account.name,
        type: t.type,
        label: t.type,
        amount: t.amount,
        timestamp: t.timestamp,
      });
    });
    trades.forEach((t) => {
      events.push({
        id: t.id,
        account: account.name,
        type: t.type,
        label: `${t.type} ${t.ticker}`,
        amount: t.shares * t.price,
        timestamp: t.timestamp,
      });
    });
  });

  events.sort((a, b) => (b.timestamp > a.timestamp ? 1 : -1));
  console.log(events);

  const recent = events.slice(0, ACTIVITY_LIMIT);
  if (!recent.length) {
    container.innerHTML = '<div class="empty">no activity</div>';
    return;
  }

  container.innerHTML = `
          <table>
            <thead>
              <tr>
                <th>account</th>
                <th>type</th>
                <th class="right">amount</th>
                <th class="right">time</th>
              </tr>
            </thead>
            <tbody>
              ${recent
                .map(
                  (e) => `
                    <tr>
                      <td class="label-col">${e.account}</td>
                      <td><span class="pill ${PILL_CLASS[e.type] || ""}">${e.label}</span></td>
                      <td class="right">${formatPrice(e.amount)}</td>
                      <td class="right">${formatDate(e.timestamp)}</td>
                    </tr>
                  `,
                )
                .join("")}
            </tbody>
          </table>
        `;
}

async function load() {
  document.getElementById("timestamp").textContent = "loading...";

  try {
    const accounts = await fetchJson("/accounts");

    const [
      detailsByAccount,
      holdingsByAccount,
      transactionsByAccount,
      tradesByAccount,
    ] = await Promise.all([
      Promise.all(accounts.map((a) => fetchJson(`/accounts/${a.name}`))),
      Promise.all(
        accounts.map((a) =>
          BROKERAGE_TYPES.has(a.type)
            ? fetchJson(`/accounts/${a.name}/holdings`).catch(() => [])
            : Promise.resolve([]),
        ),
      ),
      Promise.all(
        accounts.map((a) =>
          fetchJson(`/accounts/${a.name}/transactions`).catch(() => []),
        ),
      ),
      Promise.all(
        accounts.map((a) =>
          BROKERAGE_TYPES.has(a.type)
            ? fetchJson(`/accounts/${a.name}/trades`).catch(() => [])
            : Promise.resolve([]),
        ),
      ),
    ]);

    const accountData = accounts.map((a, i) => ({
      account: a,
      detail: detailsByAccount[i],
      holdings: holdingsByAccount[i],
      transactions: transactionsByAccount[i],
      trades: tradesByAccount[i],
      basis: costBasisMap(tradesByAccount[i]),
    }));

    renderSummary(accountData);
    renderAccounts(accountData);
    renderHoldings(accountData);
    renderActivity(accountData);

    const now = new Date();
    document.getElementById("timestamp").textContent =
      "as of " +
      now.toLocaleDateString("en-US", {
        month: "long",
        day: "numeric",
        year: "numeric",
      }) +
      " · " +
      now.toLocaleTimeString("en-US", {
        hour: "2-digit",
        minute: "2-digit",
        hour12: false,
      });
  } catch (e) {
    document.getElementById("timestamp").textContent = "error: " + e.message;
  }
}

document.getElementById("refresh-button").addEventListener("click", load);
document.addEventListener("DOMContentLoaded", load);
