function formatPrice(n) {
  return (
    "$" +
    Number(n).toLocaleString("en-US", {
      minimumFractionDigits: 2,
      maximumFractionDigits: 2,
    })
  );
}

function formatShares(n) {
  return Number(n).toLocaleString("en-US", {
    minimumFractionDigits: 2,
    maximumFractionDigits: 6,
  });
}

function formatDate(ts) {
  return new Date(ts).toLocaleDateString("en-US", {
    month: "numeric",
    day: "numeric",
    year: "numeric",
  });
}
