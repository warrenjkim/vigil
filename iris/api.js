const BASE = "";

export async function fetchJson(path) {
  const response = await fetch(BASE + path);
  if (!response.ok) {
    throw new Error(`${response.status} ${path}`);
  }

  return response.json();
}

export async function postJson(path) {
  const response = await fetch(path, { method: "POST" });
  if (!response.ok) {
    throw new Error(`${response.status} ${path}`);
  }
  return response.json();
}
