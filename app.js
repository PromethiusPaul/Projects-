const salesInput = document.getElementById("sales");
const laborInput = document.getElementById("labor");
const inventoryInput = document.getElementById("inventory");
const satisfactionInput = document.getElementById("satisfaction");
const saveBtn = document.getElementById("saveBtn");
const laborPctEl = document.getElementById("laborPct");
const statusEl = document.getElementById("status");
const savedAtEl = document.getElementById("savedAt");
const historyEl = document.getElementById("history");

const STORAGE_KEY = "restaurant-dashboard-history";

function loadHistory() {
  const history = JSON.parse(localStorage.getItem(STORAGE_KEY) || "[]");
  historyEl.innerHTML = "";
  history.slice().reverse().forEach(item => {
    const li = document.createElement("li");
    li.textContent = `${item.savedAt} | Sales: $${item.sales} | Labor: ${item.laborPct}% | Alerts: ${item.inventory} | Satisfaction: ${item.satisfaction}%`;
    historyEl.appendChild(li);
  });
}

function getStatus(laborPct, inventoryAlerts, satisfaction) {
  if (laborPct > 30) return "Watch labor cost";
  if (inventoryAlerts > 5) return "Restock priority items";
  if (satisfaction < 85) return "Review guest experience issues";
  return "Healthy operations";
}

saveBtn.addEventListener("click", () => {
  const sales = Number(salesInput.value);
  const labor = Number(laborInput.value);
  const inventory = Number(inventoryInput.value);
  const satisfaction = Number(satisfactionInput.value);

  if (!sales || !labor) {
    alert("Please enter sales and labor values.");
    return;
  }

  const laborPct = ((labor / sales) * 100).toFixed(1);
  const savedAt = new Date().toLocaleString();
  const status = getStatus(Number(laborPct), inventory, satisfaction);

  laborPctEl.textContent = `${laborPct}%`;
  statusEl.textContent = status;
  savedAtEl.textContent = savedAt;

  const history = JSON.parse(localStorage.getItem(STORAGE_KEY) || "[]");
  history.push({ sales, labor, inventory, satisfaction, laborPct, savedAt });
  localStorage.setItem(STORAGE_KEY, JSON.stringify(history));

  loadHistory();
});

loadHistory();
