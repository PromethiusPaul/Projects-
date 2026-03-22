from dataclasses import dataclass, asdict
from pathlib import Path
import json

DATA_FILE = Path("tasks.json")

@dataclass
class Task:
    title: str
    assignee: str
    priority: str
    completed: bool = False

def load_tasks():
    if not DATA_FILE.exists():
        return []
    return [Task(**item) for item in json.loads(DATA_FILE.read_text())]

def save_tasks(tasks):
    DATA_FILE.write_text(json.dumps([asdict(task) for task in tasks], indent=2))

def add_task(tasks):
    title = input("Task title: ").strip()
    assignee = input("Assignee: ").strip()
    priority = input("Priority (Low/Medium/High): ").strip() or "Medium"
    tasks.append(Task(title=title, assignee=assignee, priority=priority))
    save_tasks(tasks)
    print("Task added.\n")

def list_tasks(tasks):
    if not tasks:
        print("No tasks found.\n")
        return
    for i, task in enumerate(tasks, start=1):
        status = "Done" if task.completed else "Open"
        print(f"{i}. {task.title} | {task.assignee} | {task.priority} | {status}")
    print()

def complete_task(tasks):
    list_tasks(tasks)
    if not tasks:
        return
    try:
        index = int(input("Enter task number to mark complete: ")) - 1
        tasks[index].completed = True
        save_tasks(tasks)
        print("Task completed.\n")
    except (ValueError, IndexError):
        print("Invalid selection.\n")

def menu():
    tasks = load_tasks()
    while True:
        print("=== Shift Manager ===")
        print("1. Add task")
        print("2. List tasks")
        print("3. Complete task")
        print("4. Exit")
        choice = input("Choose an option: ").strip()

        if choice == "1":
            add_task(tasks)
        elif choice == "2":
            list_tasks(tasks)
        elif choice == "3":
            complete_task(tasks)
        elif choice == "4":
            break
        else:
            print("Unknown option.\n")

if __name__ == "__main__":
    menu()
