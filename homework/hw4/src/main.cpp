/*
    Stet Vasile-Alexandru, Grupa 10

    Am implementat un algoritm pentru unirea a k liste sortate inlantuite folosind un min-heap de dimensiune k.

    Functia `generateLists` creaza k liste sortate inlantuite cu un total de n elemente, distribuite aleatoriu intre liste (fiecare lista contine cel putin un element).

    Folosesc o structura `Node` simpla pentru liste inlantuite unilaterale. Listele sunt create in ordine inversa pentru a simula
    inserari la inceput, dar fiind sortate, ordinea finala este crescatoare.

    Complexitati:
        Timp: O(n log k)
        Spatiu: O(k) pentru heap, plus O(n) pentru lista rezultata si listele de input.

    Observatii:
        Atat pentru k fix (5, 10, 100) si n variabil (100 la 10000, pas 100), cat si pentru n fix (10000) si k variabil (10 la 500, pas 10),
        numarul de operatii creste liniar cu n si logaritmic cu k.
*/

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Profiler.h"

#define MAX_K 500
#define MAX_N 10000

Profiler p;

enum testMode {
    GENERATE = 0,
    MERGE_K_2 = 1,
    MERGE = 2
};

struct Node {
    int value;
    Node* next;
};

struct MinHeap {
    Node* arr[MAX_K];
    int size; 
    int capacity;             
};

Node* createNode(int val) {
    Node* newNode = new Node();
    newNode->value = val;
    newNode->next = nullptr;
    return newNode;
}

void insertFirst(Node** head, int val) {
    Node* newNode = createNode(val);
    newNode->next = *head;
    *head = newNode;
}

Node* createList(int a[], int n) {
    Node* head = nullptr;
    for (int i = n - 1; i >= 0; i--) {
        insertFirst(&head, a[i]);
    }
    return head;
}

void printList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        printf("%d -> ", current->value);
        current = current->next;
    }
    printf("nullptr\n");
}

void freeList(Node* head) {
    Node* current = head;
    Node* nextNode;
    while (current != nullptr) {
        nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

void generateLists(Node* lists[], int k, int n) {
    if (k <= 0) return;
    if (n < k) {
        return;
    }
    int* sizes = new int[k];

    for (int i = 0; i < k; i++) {
        sizes[i] = 1;
    }

    int remaining = n - k;

    for (int i = 0; i < remaining; i++) {
        int list_index = rand() % k;
        sizes[list_index]++;
    }

    for (int i = 0; i < k; i++) {
        int size = sizes[i]; 
        
        int* a = new int[size];
        FillRandomArray(a, size, 10, 50000, false, ASCENDING); 
        lists[i] = createList(a, size);
        delete[] a;
    }
    delete[] sizes;
}

void printLists(Node* lists[], int k) {
    for (int i = 0; i < k; i++) {
        printf("List %d: ", i);
        printList(lists[i]);
    }
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = new MinHeap();
    minHeap->size = 0;
    minHeap->capacity = capacity;
    return minHeap;
}

void swapNodes(Node **a, Node **b) {
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeap* h, int i, Operation operationCounter) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < h->size) {
        operationCounter.count();
        if (h->arr[left]->value < h->arr[smallest]->value) {
            smallest = left;
        }
    }

    if (right < h->size) {
        operationCounter.count();
        if (h->arr[right]->value < h->arr[smallest]->value) {
            smallest = right;
        }
    }

    if (smallest != i) {
        operationCounter.count(3);
        swapNodes(&h->arr[smallest], &h->arr[i]);
        minHeapify(h, smallest, operationCounter);
    }
}

void insertMinHeap(MinHeap* h, Node* node, Operation operationCounter) {
    if (h->size >= h->capacity) {
        return;
    }

    operationCounter.count();
    h->arr[h->size] = node;

    h->size++;

    int i = h->size - 1;

    while (i > 0) {
        operationCounter.count();
        if (h->arr[i]->value < h->arr[(i - 1) / 2]->value) {
            operationCounter.count(3);
            swapNodes(&h->arr[i], &h->arr[(i - 1) / 2]);
            i = (i - 1) / 2;
        } else {
            break;
        }

    }
}

Node* extractMin(MinHeap* h, Operation operationCounter) {
    if (h->size == 0) {
        return nullptr;
    }

    operationCounter.count();
    Node* root = h->arr[0];

    if (h->size > 1) {
        operationCounter.count();
        h->arr[0] = h->arr[h->size - 1];
        h->size--;
        minHeapify(h, 0, operationCounter);
    } else {
        h->size--;
    }

    return root;
}

Node* mergeKLists(Node* lists[], int k, Operation operationCounter) {
    MinHeap* h = createMinHeap(k); 

    for (int i = 0; i < k; i++) {
        if (lists[i] != nullptr) {
            insertMinHeap(h, lists[i], operationCounter);
        }
    }

    Node* dummyHead = createNode(-1); 
    Node* currentTail = dummyHead;

    while (h->size > 0) {
        Node* minNode = extractMin(h, operationCounter);

        currentTail->next = minNode;
        currentTail = minNode; 

        if (minNode->next != nullptr) {
            insertMinHeap(h, minNode->next, operationCounter);
        }
    }

    Node* mergedHead = dummyHead->next;
    delete dummyHead;
    delete h;

    return mergedHead;
}

void demo(int testMode) {
    switch (testMode) {
        case GENERATE: {
            int k = 5; 
            int n = 20; 

            Node* lists[k];
            generateLists(lists, k, n);
            printLists(lists, k);

            for (int i = 0; i < k; i++) {
                freeList(lists[i]);
            }
            break;
        }
        case MERGE_K_2: {
            int k = 2; 
            int n = 20; 

            Operation dummyCounter = p.createOperation("dummy-counter", n);

            Node* lists[k];
            generateLists(lists, k, n);
            printf("Input Lists:\n");
            printLists(lists, k);

            Node* mergedList = mergeKLists(lists, k, dummyCounter);
            printf("Merged List:\n");
            printList(mergedList);

            freeList(mergedList);
            break;
        }
        case MERGE: {
            int k = 5; 
            int n = 20; 

            Operation dummyCounter = p.createOperation("dummy-counter", n);

            Node* lists[k];
            generateLists(lists, k, n);
            printf("Input Lists:\n");
            printLists(lists, k);

            Node* mergedList = mergeKLists(lists, k, dummyCounter);
            printf("Merged List:\n");
            printList(mergedList);

            freeList(mergedList);
            break;
        }
        default:
            break;
    }
}

void demo_all() {
    demo(GENERATE);
    demo(MERGE_K_2);
    demo(MERGE);
}

void perf() {
    int k_values[] = {5, 10, 100};
    char* names[10] = {};

    for (int i = 0; i < 3; i++) {
        int k = k_values[i];
        Node* lists[k];

        names[i] = new char[32];
        snprintf(names[i], 32, "merge-k-%d", k);

        for (int n = 100; n <= MAX_N; n += 100) {
            Operation mergeOp = p.createOperation(names[i], n);
            generateLists(lists, k, n);
            Node* mergedList = mergeKLists(lists, k, mergeOp);
            freeList(mergedList);
        }
    }
    p.createGroup("Merge K Lists Operations", names[0], names[1], names[2], names[3], names[4], names[5], names[6], names[7], names[8], names[9]);

    for (int k = 10; k <= 500; k += 10) {
        Node* lists[k];

        Operation mergeOp = p.createOperation("merge-k-variable", k);
        generateLists(lists, k, MAX_N);
        Node* mergedList = mergeKLists(lists, k, mergeOp);
        freeList(mergedList);
    }
    p.showReport();
}

int main() {
    srand(time(nullptr));
    // printf("Hello World!\n");
    // demo(GENERATE);
    // demo(MERGE_K_2);
    // demo(MERGE);
    demo_all();
    // perf();
    return 0;
}