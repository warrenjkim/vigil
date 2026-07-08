const NAME = new URLSearchParams(window.location.search).get("name");

function renderSummary(account, detail, holdings, basis) {
  const isBrokerage = BROKERAGE_TYPES.has(account.type);
  const value = isBrokerage
    ? holdings.reduce((s, h) => s + (basis[h.ticker] || 0), 0)
    : detail.balance;

  const extraMetrics = isBrokerage
    ? `
        <div class="metric">
          <div class="label">positions</div>
          <div class="value">${holdings.length}</div>
        </div>
        <div class="metric pending">
          <div class="label">market value</div>
          <div class="value">TODO(implement)</div>
        </div>
      `
    : "";

  document.getElementById("summary").innerHTML = `
    <div class="metric">
      <div class="label">type</div>
      <div class="value">${account.type}</div>
    </div>
    <div class="metric">
      <div class="label">${isBrokerage ? "cost basis" : "balance"}</div>
      <div class="value ${isBrokerage ? "portfolio" : "cash"}">${
        value != null ? formatPrice(value) : "—"
      }</div>
    </div>
    ${extraMetrics}
  `;
}

function renderHoldings(holdings, basis) {
  const container = document.getElementById("holdings-container");
  if (!holdings.length) {
    container.innerHTML = '<div class="empty">no holdings</div>';
    return;
  }

  container.innerHTML = `
    <table>
      <thead>
        <tr>
          <th>ticker</th>
          <th class="right">shares</th>
          <th class="right">cost basis</th>
        </tr>
      </thead>
      <tbody>
        ${holdings
          .map(
            (h) => `
              <tr>
                <td class="label-col">${h.ticker}</td>
                <td class="right">${formatShares(h.shares)}</td>
                <td class="right">${formatPrice(basis[h.ticker] || 0)}</td>
              </tr>
            `,
          )
          .join("")}
      </tbody>
    </table>
  `;
}

function renderActivity(transactions, trades) {
  const container = document.getElementById("activity-container");
  const events = [];

  transactions.forEach((t) => {
    events.push({
      type: t.type,
      label: t.type,
      merchant: t.merchant,
      amount: t.amount,
      timestamp: t.timestamp,
    });
  });
  trades.forEach((t) => {
    events.push({
      type: t.type,
      label: `${t.type} ${t.ticker}`,
      merchant: "--",
      amount: t.shares * t.price,
      timestamp: t.timestamp,
    });
  });

  events.sort((a, b) => (b.timestamp > a.timestamp ? 1 : -1));

  if (!events.length) {
    container.innerHTML = '<div class="empty">no activity</div>';
    return;
  }

  container.innerHTML = `
    <table>
      <thead>
        <tr>
          <th>type</th>
          <th>merchant</th>
          <th class="right">amount</th>
          <th class="right">time</th>
        </tr>
      </thead>
      <tbody>
        ${events
          .map(
            (e) => `
              <tr>
                <td><span class="pill ${PILL_CLASS[e.type] || ""}">${e.label}</span></td>
                <td class="label-col">${e.merchant}</td>
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
  if (!NAME) {
    document.getElementById("timestamp").textContent = "error: missing name";
    return;
  }

  document.getElementById("account-name").textContent = "iris // " + NAME;
  document.getElementById("timestamp").textContent = "loading...";

  try {
    const account = await fetchJson(`/accounts/${NAME}`);
    const isBrokerage = BROKERAGE_TYPES.has(account.type);

    const [holdings, transactions, trades] = await Promise.all([
      isBrokerage
        ? fetchJson(`/accounts/${NAME}/holdings`).catch(() => [])
        : Promise.resolve([]),
      fetchJson(`/accounts/${NAME}/transactions`).catch(() => []),
      isBrokerage
        ? fetchJson(`/accounts/${NAME}/trades`).catch(() => [])
        : Promise.resolve([]),
    ]);

    const basis = costBasisMap(trades);

    document
      .getElementById("summary")
      .classList.toggle("two-metric", !isBrokerage);
    document
      .getElementById("detail-sections")
      .classList.toggle("single-col", !isBrokerage);
    document.getElementById("holdings-section").style.display = isBrokerage
      ? ""
      : "none";

    renderSummary(account, account, holdings, basis);
    if (isBrokerage) {
      renderHoldings(holdings, basis);
    }

    renderActivity(transactions, trades);

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

document.addEventListener("DOMContentLoaded", load);
