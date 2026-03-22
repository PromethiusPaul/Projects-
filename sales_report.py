import csv
from pathlib import Path

INPUT_FILE = Path("sample_sales.csv")

def load_rows(path):
    with path.open(newline="") as f:
        reader = csv.DictReader(f)
        return list(reader)

def summarize(rows):
    total_sales = sum(float(row["sales"]) for row in rows)
    total_labor = sum(float(row["labor_cost"]) for row in rows)
    avg_satisfaction = sum(float(row["guest_satisfaction"]) for row in rows) / len(rows)
    labor_pct = (total_labor / total_sales) * 100 if total_sales else 0
    highest_day = max(rows, key=lambda r: float(r["sales"]))
    return {
        "total_sales": round(total_sales, 2),
        "total_labor": round(total_labor, 2),
        "avg_satisfaction": round(avg_satisfaction, 1),
        "labor_pct": round(labor_pct, 1),
        "best_day": highest_day["date"],
    }

def main():
    rows = load_rows(INPUT_FILE)
    summary = summarize(rows)

    print("=== Weekly Sales Summary ===")
    print(f"Total Sales: ${summary['total_sales']}")
    print(f"Total Labor: ${summary['total_labor']}")
    print(f"Labor %: {summary['labor_pct']}%")
    print(f"Average Guest Satisfaction: {summary['avg_satisfaction']}%")
    print(f"Best Sales Day: {summary['best_day']}")

if __name__ == "__main__":
    main()
