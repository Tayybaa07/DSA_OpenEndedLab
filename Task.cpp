/*
 * 1) Binary Search Tree (BST)  stores student records, keyed by unique roll number.
 *     Searching/inserting by roll number needs no scanning of every record;
 *      each comparison discards one whole subtree (fast when tree is balanced).
 *     An in-order traversal gives records sorted by roll number for free.
 *     Being linked, it never becomes "full" unlike a fixed size array.
 *
 * 2) Queue (linked list with front and rear pointers)  stores help requests (roll numbers).
 *    - Help desk must serve the EARLIEST waiting request first = FIFO,
 *      which is exactly what a queue provides.
 *    - Enqueue (at rear) and dequeue (at front) are both O(1).
 *    - Repeated requests from the same student are allowed (duplicates are fine).
 *
 * COMPLEXITY (n = number of students, h = height of the BST)
 *  Add student    : worst case O(n)  (BST degenerates into a chain if roll numbers
 *                   are inserted in sorted order, so h = n). Average O(log n).
 *  Search student : worst case O(n)  (same reason). Average O(log n).
 *  Display all    : O(n).
 *  Enqueue/Dequeue help request : O(1)  (the registration check before enqueue costs
 *                   one BST search, i.e. O(n) worst case).
 *
 * Alternative data structure: a hash table (roll number -> record) gives O(1) average
 * add/search, but does not keep records sorted for display, a balanced BST (AVL)
 *  guarantees O(log n) worst case for add and search.
 *
 */

#include <iostream>
#include <cstring>
using namespace std;

const int NAME_LEN = 50;

//BST of students 
struct Student {
    int rollNo;
    char name[NAME_LEN];
    Student *left, *right;
};

Student* createStudent(int roll, const char* name) {
    Student* s = new Student;
    s->rollNo = roll;
    strncpy(s->name, name, NAME_LEN - 1);
    s->name[NAME_LEN - 1] = '\0';
    s->left = s->right = NULL;
    return s;
}

// Search
Student* searchStudent(Student* root, int roll) {
    while (root != NULL) {
        if (roll == root->rollNo) return root;
        root = (roll < root->rollNo) ? root->left : root->right;
    }
    return NULL;
}

// Insert
bool addStudent(Student*& root, int roll, const char* name) {
    if (root == NULL) {
        root = createStudent(roll, name);
        return true;
    }
    Student* cur = root;
    while (true) {
        if (roll == cur->rollNo) return false;          // duplicate
        Student*& next = (roll < cur->rollNo) ? cur->left : cur->right;
        if (next == NULL) {
            next = createStudent(roll, name);
            return true;
        }
        cur = next;
    }
}

// In order traversal
void displayStudents(Student* root) {
    if (root == NULL) return;
    displayStudents(root->left);
    cout << "  Roll No: " << root->rollNo << "  |  Name: " << root->name << endl;
    displayStudents(root->right);
}

void freeStudents(Student* root) {                     
    if (root == NULL) return;
    freeStudents(root->left);
    freeStudents(root->right);
    delete root;
}

// Queue of help requests 
struct Node {
    int rollNo;
    Node* next;
};

struct Queue {
    Node *front, *rear;
    int size;
    Queue() : front(NULL), rear(NULL), size(0) {}
};

bool isQueueEmpty(const Queue& q) { return q.front == NULL; }

void enqueue(Queue& q, int roll) {                        
    Node* n = new Node;
    n->rollNo = roll;
    n->next = NULL;
    if (q.rear == NULL) q.front = q.rear = n;
    else { q.rear->next = n; q.rear = n; }
    q.size++;
}

bool dequeue(Queue& q, int& roll) {                      
    if (isQueueEmpty(q)) return false;
    Node* temp = q.front;
    roll = temp->rollNo;
    q.front = q.front->next;
    if (q.front == NULL) q.rear = NULL;
    delete temp;
    q.size--;
    return true;
}
//Display Queue Function
void displayQueue(const Queue& q) {
    if (isQueueEmpty(q)) { cout << "  No waiting help requests.\n"; return; }
    cout << "  Waiting requests (front -> rear): ";
    for (Node* p = q.front; p != NULL; p = p->next) {
        cout << p->rollNo << (p->next ? " -> " : "");
    }
    cout << "  [" << q.size << " waiting]\n";
}

void freeQueue(Queue& q) {
    int dummy;
    while (dequeue(q, dummy)) {}
}

// Menu helpers 
void readName(char* name) {
    cin.ignore(1000, '\n');                              
    cin.getline(name, NAME_LEN);
}

void doAdd(Student*& root) {
    int roll; char name[NAME_LEN];
    cout << "Enter roll number: "; cin >> roll;
    cout << "Enter name: ";        readName(name);
    if (addStudent(root, roll, name))
        cout << "Student added successfully.\n";
    else
        cout << "Error: roll number " << roll << " already exists. Student not added.\n";
}

void doSearch(Student* root) {
    int roll;
    cout << "Enter roll number to search: "; cin >> roll;
    if (root == NULL) { cout << "No students stored yet.\n"; return; }
    Student* s = searchStudent(root, roll);
    if (s) cout << "Found -> Roll No: " << s->rollNo << ", Name: " << s->name << endl;
    else   cout << "Student with roll number " << roll << " not found.\n";
}

void doDisplay(Student* root) {
    if (root == NULL) { cout << "No students to display (list is empty).\n"; return; }
    cout << "Students (sorted by roll number):\n";
    displayStudents(root);
}

void doHelpRequest(Student* root, Queue& q) {
    int choice;
    cout << "  1. Add help request\n  2. Serve earliest request\n  3. Show waiting requests\n"
         << "  Choice: ";
    cin >> choice;
    if (choice == 1) {
        int roll;
        cout << "Enter roll number requesting help: "; cin >> roll;
        if (searchStudent(root, roll) == NULL)
            cout << "Rejected: roll number " << roll << " is not a registered student.\n";
        else {
            enqueue(q, roll);
            cout << "Help request added for roll number " << roll << ".\n";
        }
    } else if (choice == 2) {
        int roll;
        if (!dequeue(q, roll)) cout << "No pending help requests (queue is empty).\n";
        else {
            Student* s = searchStudent(root, roll);
            cout << "Now serving -> Roll No: " << roll
                 << ", Name: " << (s ? s->name : "?") << endl;
        }
    } else if (choice == 3) {
        displayQueue(q);
    } else {
        cout << "Invalid choice.\n";
    }
}
//Show Menu Function
void showMenu() {
    cout << "\n STUDENT HELP DESK \n"
         << "1. Add student\n"
         << "2. Search student by roll number\n"
         << "3. Display all students\n"
         << "4. Help requests (queue)\n"
         << "5. Exit\n"
         << "Enter choice: ";
}
//Main Function

int main() {
    Student* root = NULL;
    Queue helpQueue;
    int choice;

    do {
        showMenu();
        cin >> choice;
        switch (choice) {
            case 1: doAdd(root); break;
            case 2: doSearch(root); break;
            case 3: doDisplay(root); break;
            case 4: doHelpRequest(root, helpQueue); break;
            case 5: cout << "Releasing memory and exiting...\n"; break;
            default: cout << "Invalid option, try again.\n";
        }
    } while (choice != 5);

    freeStudents(root);          
    freeQueue(helpQueue);     
    return 0;
}