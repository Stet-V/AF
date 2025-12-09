/*
    Stet Vasile-Alexandru, Grupa 10

    Am implementat algoritmul lui Kruskal pentru determinarea Arborelui Minim de Acoperire (MST - Minimum Spanning Tree),
    utilizand structuri de date pentru multimi disjuncte (Disjoint Sets). Pentru a asigura o complexitate aproape liniara 
    a operatiilor pe multimi, am folosit euristicile "Union by Rank" si "Path Compression".
    De asemenea, pentru sortarea muchiilor am implementat un algoritm Hybrid QuickSort (optimizat cu Insertion Sort).

    Structurile utilizate:
    - Node (pentru multimi disjuncte): 
        - parent: pointer catre parintele nodului
        - rank: rangul (inaltimea estimata a subarborelui) folosit pentru balansare la reuniune
        - value: valoarea retinuta
    - Edge (pentru graf):
        - u, v: nodurile extremitati
        - weight: costul muchiei

    Operatii principale:
    1. Disjoint Set Operations:
       - makeSet(x, value): Creeaza o multime noua cu un singur element (x devine propriul parinte, rank 0). Complexitate O(1).
       - findSet(x): Returneaza reprezentantul multimii din care face parte x. Aplica "Path Compression" (leaga recursiv
         toate nodurile de pe drum direct de radacina), reducand inaltimea arborelui. Complexitate O(alpha(n)) amortizat.
       - unionSets(x, y): Reuneste multimile lui x si y. Aplica "Union by Rank" (arborele cu rang mai mic este atasat
         radacinii arborelui cu rang mai mare). Daca rangurile sunt egale, rangul radacinii rezultate creste cu 1. 
         Complexitate O(alpha(n)) amortizat.

    2. Algoritmi auxiliari:
       - hybridQuickSort: Sorteaza muchiile crescator dupa cost. Combina QuickSort cu InsertionSort (pentru partitii < 15 elemente)
         pentru eficienta practica.

    3. kruskal(nodes, edges):
       - Initializeaza n multimi disjuncte.
       - Sorteaza muchiile dupa greutate.
       - Itereaza prin muchii: daca capetele muchiei (u, v) fac parte din multimi disjuncte (findSet(u) != findSet(v)),
         muchia este adaugata la MST si multimile sunt unite.
       - Complexitate O(E log E) sau O(E log V) dominata de sortare.

    Complexitati:
    - makeSet:                     O(1)
    - findSet / unionSets:         O(alpha(n)) amortizat, unde alpha este inversa functiei Ackermann
    - Sortare muchii:              O(E log E)
    - Kruskal total:               O(E log E) sau O(E log V)
    - Spatiu total:                O(V + E) pentru stocarea grafului si a structurilor de paduri disjuncte
*/

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Profiler.h"

#define MAX_N 10000

Profiler p;

typedef struct Node {
    Node* parent;
    int rank;
    int value;
} Node;

typedef struct Edge {
    int u;
    int v;
    int weight;
} Edge;

void makeSet(Node* x, int value, Operation* op = NULL) {
    if (op) op->count(2);
    x->parent = x;
    x->rank = 0;
    x->value = value;
}

Node* findSet(Node* x, Operation* op = NULL) {
    if (op) op->count();
    if (x != x->parent) {
        if (op) op->count();
        x->parent = findSet(x->parent, op);
    }
    return x->parent;
}

void unionSets(Node* x, Node* y, Operation* opUnion = NULL, Operation* opFind = NULL) {
    Node* xRoot = findSet(x, opFind);
    Node* yRoot = findSet(y, opFind);

    if (opUnion) opUnion->count();
    if (xRoot == yRoot) return;

    if (opUnion) opUnion->count();
    if (xRoot->rank < yRoot->rank) {
        if (opUnion) opUnion->count();
        xRoot->parent = yRoot;
    } else {
        if (opUnion) opUnion->count();
        if (xRoot->rank > yRoot->rank) {
            if (opUnion) opUnion->count();
            yRoot->parent = xRoot;
        } else {
            if (opUnion) opUnion->count(2);
            yRoot->parent = xRoot;
            xRoot->rank++;
        }
    }
}

void printSets(Node* nodes[], int n) {
    Node* representatives[n];
    int rep_count = 0;
    for (int i = 0; i < n; i++) {
        Node* rep = findSet(nodes[i]);
        int found = 0;
        for (int j = 0; j < rep_count; j++) {
            if (representatives[j] == rep) {
                found = 1;
                break;
            }
        }
        if (!found) representatives[rep_count++] = rep;
    }

    for (int i = 0; i < rep_count; i++) {
        bool printComma = false;
        Node* current_rep = representatives[i];
        printf("%d: {", current_rep->value);
        for (int j = 0; j < n; j++) {
            if (findSet(nodes[j]) == current_rep) {
                if (printComma) printf(", ");
                printf("%d", nodes[j]->value);
                printComma = true;
            }
        }
        printf("}\n");
    }
}

int partition(Edge a[], int left, int right, Operation* opComp, Operation* opAssign) {
    if (opAssign) opAssign->count();
    Edge pivot = a[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; j++) {
        if (opComp) opComp->count();
        if (a[j].weight < pivot.weight) {
            i++;
            if (opAssign) opAssign->count(3);
            Edge temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }
    if (opAssign) opAssign->count(3);
    Edge temp = a[i + 1];
    a[i + 1] = a[right];
    a[right] = temp;
    return i + 1;
}

void insertionSort(Edge a[], int n, Operation* opComp, Operation* opAssign) {
    for (int i = 1; i < n; i++) {
        if (opAssign) opAssign->count();
        Edge key = a[i];
        int left = 0;
        int right = i - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (opComp) opComp->count();
            if (a[mid].weight > key.weight) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        for (int j = i - 1; j >= left; j--) {
            if (opAssign) opAssign->count();
            a[j + 1] = a[j];
        }
        if (opAssign) opAssign->count();
        a[left] = key;
    }
}

void hybridQuickSort(Edge a[], int left, int right, int threshold, Operation* opComp, Operation* opAssign) {
    if (left < right) {
        if (right - left + 1 <= threshold) {
            insertionSort(a + left, right - left + 1, opComp, opAssign);
        } else {
            int pivotIndex = partition(a, left, right, opComp, opAssign);
            hybridQuickSort(a, left, pivotIndex - 1, threshold, opComp, opAssign);
            hybridQuickSort(a, pivotIndex + 1, right, threshold, opComp, opAssign);
        }
    }
}

void kruskal(Node* nodes[], int n, Edge edges[], int m, bool print, Operation* opMake = NULL, Operation* opUnion = NULL, Operation* opFind = NULL) {
    for (int i = 0; i < n; i++) makeSet(nodes[i], i + 1, opMake);
    hybridQuickSort(edges, 0, m - 1, 15, NULL, NULL);

    int mstWeight = 0;
    if (print) printf("Kruskal MST Selected Edges:\n");
    for (int i = 0; i < m; i++) {
        Node* u = nodes[edges[i].u - 1];
        Node* v = nodes[edges[i].v - 1];
        if (findSet(u, opFind) != findSet(v, opFind)) {
            if (print) printf("(%d, %d) w:%d\n", edges[i].u, edges[i].v, edges[i].weight);
            mstWeight += edges[i].weight;
            unionSets(u, v, opUnion, opFind);
        }
    }
    if (print) printf("Total Weight: %d\n", mstWeight);
}

void demo() {
    printf("--- DISJOINT SETS ---\n");
    const int n = 10;
    Node* nodes[n];
    for (int i = 0; i < n; i++) {
        nodes[i] = new Node();
        makeSet(nodes[i], i + 1);
    }
    printSets(nodes, n);

    for (int i = 0; i < n / 2; i++) {
        int u = rand() % n;
        int v = rand() % n;
        printf("(%d, %d)\n", nodes[u]->value, nodes[v]->value);
        unionSets(nodes[u], nodes[v]);
    }

    printSets(nodes, n);
    for (int i = 0; i < n; i++) delete nodes[i];

    printf("--- KRUSKAL'S ALGORITHM ---\n");
    const int numV = 5;
    const int numE = 9;
    Node* kNodes[numV];
    for (int i = 0; i < numV; i++) kNodes[i] = new Node();

    Edge edges[numE] = {
        {1, 2, 4}, {1, 3, 2}, {1, 4, 6},
        {2, 3, 5}, {2, 4, 8}, {2, 5, 10},
        {3, 4, 3}, {3, 5, 1}, {4, 5, 9}
    };

    for (int i = 0; i < numE; i++) printf("(%d, %d) w:%d\n", edges[i].u, edges[i].v, edges[i].weight);

    kruskal(kNodes, numV, edges, numE, true);

    for (int i = 0; i < numV; i++) delete kNodes[i];
}

void perf() {
    for (int n = 100; n <= MAX_N; n += 100) {
        Operation opMake = p.createOperation("MAKE_SET", n);
        Operation opUnion = p.createOperation("UNION", n);
        Operation opFind = p.createOperation("FIND_SET", n);

        Node** nodes = new Node*[n];
        for (int i = 0; i < n; i++) nodes[i] = new Node();

        int numEdges = n * 4;
        Edge* edges = new Edge[numEdges];
        int m = 0;

        for (int i = 0; i < n - 1; i++) {
            edges[m].u = i + 1;
            edges[m].v = i + 2;
            edges[m].weight = rand() % 100 + 1;
            m++;
        }

        while (m < numEdges) {
            int u = rand() % n + 1;
            int v = rand() % n + 1;
            if (u != v) {
                edges[m].u = u;
                edges[m].v = v;
                edges[m].weight = rand() % 100 + 1;
                m++;
            }
        }

        kruskal(nodes, n, edges, numEdges, false, &opMake, &opUnion, &opFind);

        for (int i = 0; i < n; i++) delete nodes[i];
        delete[] nodes;
        delete[] edges;
    }
    p.createGroup("Disjoint Set Operations", "MAKE_SET", "FIND_SET", "UNION");
    p.showReport();
}

int main() {
    srand(time(NULL));
    demo();
    // perf();
    return 0;
}