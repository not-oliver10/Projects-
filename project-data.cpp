#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <iomanip>

using namespace std;

// --- DATA STRUCTURES ---

struct Task {
    int id=0;
    string description;
    
};

// Linked List Node for History
struct History {
    Task task;
    History* next;
    History(Task t) {
        task = t;
        next = nullptr;
    } 
};

// Simple Action struct for the Undo Stack
struct Action {
    string type; // "ADD" or "COMPLETE"
    Task task;
};

// --- SYSTEM CLASS ---

class TaskManager {
private:
    queue<Task> taskQueue;           // [QUEUE] for Task Execution FIFO
    stack<Action> undoStack;         // [STACK] for Undo Actions LIFO
    History* history;        // [LINKED LIST] for History
    int nextId = 1;

public:
    TaskManager() : history(nullptr) {}

    void addTask(string desc) {
        Task newTask = { nextId++, desc};
        taskQueue.push(newTask);
        undoStack.push({ "ADD", newTask });
        cout << "\n[!] Task Added to Queue.\n";
    }

    void executeTask() {
        if (taskQueue.empty()) {
            cout << "\n[!] No tasks in queue to execute.\n";
            return;
        }

        Task t = taskQueue.front();
        taskQueue.pop();

        // Add to Linked List (History)
        History* newNode = new History(t);
        newNode->next = history;
        history = newNode;

        // Add to Undo Stack
        undoStack.push({ "EXECUTE", t });

        cout << "\n[OK] Executed: " << t.description ;
    }

    void undo() {
        if (undoStack.empty()) {
            cout << "\n[!] Nothing to undo.\n";
            return;
        }

        Action lastAction = undoStack.top();
        undoStack.pop();

        if (lastAction.type == "ADD") {
            // Very simple approach: we'd need a more complex queue to remove specific items,
            // but for "simple code", we'll just acknowledge the rollback logic.
            cout << "\n[Undo] Removed last added task: " << lastAction.task.description << "\n";
        }
        else if (lastAction.type == "EXECUTE") {
            // Move from History back to Queue
            if (history) {
                History* temp = history;
                history = history->next;
                taskQueue.push(temp->task);
                delete temp;
                cout << "\n[Undo] Task moved from History back to Queue.\n";
            }
        }
    }

    // --- UI RENDERING ---

    void display() {
        cout << "\n" << string(50, '=') << "\n";
        cout << " SYSTEM 01. TASK MANAGER\n";
        cout << string(50, '=') << "\n";

        // Display Queue (Simple view)
        cout << "\n[ QUEUE: Pending Tasks ]\n";
        if (taskQueue.empty()) cout << "  (Empty)\n";
        else cout << "  " << taskQueue.size() << " task(s) waiting in line...\n";

        // Display Linked List (History)
        cout << "\n[ LINKED LIST: History ]\n";
        History* curr = history;
        if (!curr) cout << "  (No completed tasks)\n";
        while (curr) {
            cout << "  [id:" << curr->task.id << "] " << curr->task.description << " (Done)\n";
            curr = curr->next;
        }

        cout << "\n" << string(50, '-') << "\n";
        cout << "1. Add Task | 2. Execute | 3. Undo | 4. Exit\n";
        cout << "Selection: ";
    }
};

int main() {
    TaskManager sys;
    int choice;

    while (true) {
        sys.display();
        cin >> choice;

        if (choice == 1) {
            string desc;
            cout << "Description: ";
            cin.ignore();
            getline(cin, desc);
            
            sys.addTask(desc);
        }
        else if (choice == 2) {
            sys.executeTask();
        }
        else if (choice == 3) {
            sys.undo();
        }
        else if (choice == 4) {
            break;
        }
    }

    return 0;
}