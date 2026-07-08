async function fetchJson(path) {
  const response = await fetch(BASE + path);
  if (!response.ok) {
    throw new Error(`${response.status} ${path}`);
  }

  return response.json();
}
