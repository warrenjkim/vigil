const out = document.getElementById("out");

function val(id) {
  return document.getElementById(id).value.trim();
}

function display(status, body) {
  const ok = status >= 200 && status < 300;
  out.className = ok ? "ok" : "err";
  try {
    out.textContent = `${status}\n\n${JSON.stringify(JSON.parse(body), null, 2)}`;
  } catch {
    out.textContent = `${status}\n\n${body}`;
  }
}

async function get(path) {
  try {
    const r = await fetch(path);
    display(r.status, await r.text());
  } catch (e) {
    out.className = "err";
    out.textContent = e.message;
  }
}

async function post(path, body) {
  try {
    const r = await fetch(path, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: typeof body === "string" ? body : JSON.stringify(body),
    });
    display(r.status, await r.text());
  } catch (e) {
    out.className = "err";
    out.textContent = e.message;
  }
}

async function del(path) {
  try {
    const r = await fetch(path, { method: "DELETE" });
    display(r.status, await r.text());
  } catch (e) {
    out.className = "err";
    out.textContent = e.message;
  }
}
