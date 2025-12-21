/*
    Stet Vasile-Alexandru, Grupa 10

    Am implementat algoritmii de parcurgere DFS, Sortare Topologica si 
    Tarjan pentru grafuri orientate, reprezentate prin liste de adiacenta.
    Am modificat DFS-ul pentru a servi ca baza pentru sortarea topologica si pentru a include
    o functie de detectare a ciclurilor. Am inclus functii auxiliare pentru managementul
    grafurilor, stivei si testarea de performanta.

    Structuri de Date:
    * Graf: Reprezentat prin liste de adiacenta (`Node** adjLists`).
    * Stiva: Utilizata in DFS (pentru Sortarea Topologica) si in algoritmul Tarjan (pentru a urmari nodurile).
    * Stari de Vizitare: UNVISITED, VISITING, VISITED.
    * TarjanNode: Stocheaza `index`, `lowlink` si starea `onStack` necesare pentru Tarjan.

    Functii implementate:
    1. dfsVisit(graph, u, visited, parent, op, depth, demo, topoStack):
    Functia de baza a DFS-ului, recursiva.
    * Realizeaza parcurgerea in adancime, actualizand starile (UNVISITED -> VISITING -> VISITED).
    * Detecteaza ciclurile (muchie catre nod VISITING) si returneaza `false` daca gaseste un ciclu.
    * Daca `demo` este `true`, afiseaza nodul cu indentare pentru a vizualiza arborele DFS.
    * Daca `topoStack` nu este `NULL`, adauga nodul pe stiva dupa ce toti descendentii au fost vizitati (`VISITED`), formand ordinea inversa a sortarii topologice.
    Complexitate: O(V + E) pe un graf conex.

    2. dfs(graph, demo, op, topoStack):
    Functia initializatoare DFS.
    * Initializeaza starile de vizitare si vectorul de parinti.
    * Itereaza peste toate nodurile pentru a asigura parcurgerea componentelor neconexe.
    * Returneaza `true` daca graful este DAG (nu are cicluri), `false` altfel.
    Complexitate: O(V + E).

    3. topologicalSort(graph):
    Calculeaza si afiseaza sortarea topologica.
    * Foloseste `dfs` cu un parametru `topoStack` non-NULL pentru a efectua parcurgerea si a popula stiva in ordinea corecta (inversa).
    * Daca `dfs` returneaza `false` (ciclu detectat), sortarea nu este posibila.
    * Altfel, extrage elementele din stiva pentru a afisa rezultatul final.
    Complexitate: O(V + E).

    4. strongConnect(graph, v, tNodes, s, index):
    Functia recursiva a algoritmului Tarjan pentru gasirea Componentelor Tare Conexe (SCC).
    * Calculeaza `index` (timpul de descoperire) si `lowlink` (nodul cu cel mai mic index accesibil din subarbore, inclusiv prin muchii inapoi).
    * Utilizeaza o stiva (`s`) pentru a urmari nodurile aflate in parcurgerea curenta.
    * Cand `tNodes[v].lowlink == tNodes[v].index`, s-a gasit o SCC, care este apoi extrasa din stiva si afisata.
    Complexitate: O(V + E) pe un graf conex.

    5. tarjan(graph):
    Functia initializatoare Tarjan.
    * Initializeaza structurile `TarjanNode` si itereaza peste noduri pentru a parcurge toate componentele.
    Complexitate: O(V + E).

    6. generateRandomGraph(graph, E):
    Genereaza un graf orientat aleator cu V noduri si E muchii unice, folosind o matrice de adiacenta temporara pentru a verifica unicitatea.
    Complexitate: Timp amortizat O(E), dar in cel mai rau caz (densitate mare) poate fi mai mare.

    7. perf():
    Testeaza performanta DFS (bazata pe operatii elementare de vizitare si iterare a muchiilor) pentru a demonstra complexitatea O(V + E):
    * Variaza numarul de muchii (E) mentinand V=100 fix.
    * Variaza numarul de noduri (V) mentinand E=4500 fix.

    Spatiu total: O(V + E) pentru listele de adiacenta, stari de vizitare, parinti si structurile auxiliare (Stack, TarjanNode).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Profiler.h"

#define MAX_NODES 10000

enum VisitState {
    UNVISITED = 0,
    VISITING = 1,
    VISITED = 2
};

typedef struct Node {
    int v;
    struct Node* next;
} Node;

typedef struct Graph {
    int numVertices;
    Node** adjLists;
} Graph;

typedef struct Stack {
    int items[MAX_NODES];
    int top;
} Stack;

typedef struct TarjanNode {
    int index;
    int lowlink;
    bool onStack;
    bool defined;
} TarjanNode;

Node* createNode(int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->v = v;
    newNode->next = NULL;
    return newNode;
}

Graph* createGraph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->adjLists = (Node**)malloc(vertices * sizeof(Node*));
    for (int i = 0; i < vertices; i++) {
        graph->adjLists[i] = NULL;
    }
    return graph;
}

void addEdge(Graph* graph, int src, int dest) {
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

void freeGraph(Graph* graph) {
    for (int i = 0; i < graph->numVertices; i++) {
        Node* temp = graph->adjLists[i];
        while (temp) {
            Node* toFree = temp;
            temp = temp->next;
            free(toFree);
        }
    }
    free(graph->adjLists);
    free(graph);
}

void printGraph(Graph* graph) {
    for (int i = 0; i < graph->numVertices; i++) {
        printf("%d: ", i);
        Node* temp = graph->adjLists[i];
        while (temp) {
            printf("%d -> ", temp->v);
            temp = temp->next;
        }
        printf("NULL\n");
    }
    printf("\n");
}

void initStack(Stack* s) {
    s->top = -1;
}

bool isStackEmpty(Stack* s) {
    return s->top == -1;
}

void push(Stack* s, int value) {
    if (s->top < MAX_NODES - 1) {
        s->items[++s->top] = value;
    }
}

int pop(Stack* s) {
    if (!isStackEmpty(s)) {
        return s->items[s->top--];
    }
    return -1;
}

bool dfsVisit(Graph* graph, int u, int* visited, int* parent, Operation* op, int depth, bool demo, Stack* topoStack) {
    if (op != NULL) op->count();
    
    visited[u] = VISITING;

    if (demo && op == NULL && topoStack == NULL) {
        for (int k = 0; k < depth; k++) printf("    ");
        printf("%d\n", u);
    }
    
    Node* temp = graph->adjLists[u];
    bool isDag = true;

    while (temp) {
        if (op != NULL) op->count();
        int v = temp->v;
        
        if (visited[v] == VISITING) {
            isDag = false;
        }

        if (visited[v] == UNVISITED) {
            parent[v] = u;
            if (!dfsVisit(graph, v, visited, parent, op, depth + 1, demo, topoStack)) {
                isDag = false;
            }
        }
        temp = temp->next;
    }

    visited[u] = VISITED;
    
    if (topoStack != NULL) {
        push(topoStack, u);
    }

    return isDag;
}

bool dfs(Graph* graph, bool demo, Operation* op, Stack* topoStack) {
    int* visited = (int*)malloc(graph->numVertices * sizeof(int));
    int* parent = (int*)malloc(graph->numVertices * sizeof(int));
    bool isPossible = true;
    
    for (int i = 0; i < graph->numVertices; i++) {
        visited[i] = UNVISITED;
        parent[i] = -1;
    }

    for (int i = 0; i < graph->numVertices; i++) {
        if (visited[i] == UNVISITED) {
            if (!dfsVisit(graph, i, visited, parent, op, 0, demo, topoStack)) {
                isPossible = false;
            }
        }
    }

    if (demo && op == NULL && topoStack == NULL) {
        printf("\n");
    }

    free(visited);
    free(parent);

    return isPossible;
}

void topologicalSort(Graph* graph) {
    Stack stack;
    initStack(&stack);
    
    bool isDag = dfs(graph, false, NULL, &stack);

    if (!isDag) {
        printf("Topological Sort: ");
        while (!isStackEmpty(&stack)) {
            printf("%d ", pop(&stack));
        }
        printf("\n\n");
    } else {
        printf("Topological Sort: Impossible\n\n");
    }
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

void strongConnect(Graph* graph, int v, TarjanNode* tNodes, Stack* s, int* index) {
    tNodes[v].index = *index;
    tNodes[v].lowlink = *index;
    tNodes[v].defined = true;
    (*index)++;
    push(s, v);
    tNodes[v].onStack = true;

    Node* temp = graph->adjLists[v];
    while (temp) {
        int w = temp->v;
        if (!tNodes[w].defined) {
            strongConnect(graph, w, tNodes, s, index);
            tNodes[v].lowlink = min(tNodes[v].lowlink, tNodes[w].lowlink);
        } else if (tNodes[w].onStack) {
            tNodes[v].lowlink = min(tNodes[v].lowlink, tNodes[w].index);
        }
        temp = temp->next;
    }

    if (tNodes[v].lowlink == tNodes[v].index) {
        printf("{");
        int w;
        bool printComma = false;
        do {
            w = pop(s);
            tNodes[w].onStack = false;
            if (printComma) printf(", ");
            printComma = true;
            printf("%d", w);
        } while (w != v);
        printf("}\n");
    }
}

void tarjan(Graph* graph) {
    printf("Tarjan:\n");
    int V = graph->numVertices;
    int index = 0;
    Stack tarjanStack;
    initStack(&tarjanStack);
    TarjanNode* tNodes = (TarjanNode*)malloc(V * sizeof(TarjanNode));
    
    for (int i = 0; i < V; i++) {
        tNodes[i].index = -1;
        tNodes[i].lowlink = -1;
        tNodes[i].onStack = false;
        tNodes[i].defined = false;
    }

    for (int i = 0; i < V; i++) {
        if (!tNodes[i].defined) {
            strongConnect(graph, i, tNodes, &tarjanStack, &index);
        }
    }
    printf("\n");
    free(tNodes);
}

void generateRandomGraph(Graph* graph, int E) {
    int V = graph->numVertices;
    int edgesCount = 0;
    int** mat = (int**)malloc(V * sizeof(int*));
    
    for (int i = 0; i < V; i++) {
        mat[i] = (int*)calloc(V, sizeof(int));
    }

    while (edgesCount < E) {
        int u = rand() % V;
        int v = rand() % V;
        if (u != v && mat[u][v] == 0) {
            mat[u][v] = 1;
            addEdge(graph, u, v);
            edgesCount++;
        }
    }

    for (int i = 0; i < V; i++) {
        free(mat[i]);
    }
    free(mat);
}

void demoCyclic() {
    Graph* g = createGraph(7);

    addEdge(g, 0, 1);
    addEdge(g, 1, 2);
    addEdge(g, 2, 0);
    addEdge(g, 2, 3); 
    addEdge(g, 4, 5);
    addEdge(g, 5, 4);
    addEdge(g, 3, 4); 
    addEdge(g, 6, 6);
    addEdge(g, 0, 6);

    printGraph(g);

    dfs(g, true, NULL, NULL); 
    topologicalSort(g);
    
    tarjan(g);

    freeGraph(g);
}

void demoAcyclic() {
    Graph* g = createGraph(6);

    addEdge(g, 5, 0);
    addEdge(g, 5, 2);
    addEdge(g, 4, 0);
    addEdge(g, 4, 1);
    addEdge(g, 2, 3);
    addEdge(g, 3, 1);

    printGraph(g);

    dfs(g, true, NULL, NULL);
    topologicalSort(g);

    tarjan(g);

    freeGraph(g);
}

void demo() {
    demoCyclic();
    demoAcyclic();
}

void perf() {
    Profiler p("dfs_analysis"); 

    int fixedV = 100;

    for (int E = 1000; E <= 4500; E += 100) {
        Operation op = p.createOperation("dfs-edges", E);
        Graph* g = createGraph(fixedV);
        generateRandomGraph(g, E);
        
        dfs(g, false, &op, NULL); 
        
        freeGraph(g);
    }
    
    int fixedE = 4500;
    
    for (int V = 100; V <= 200; V += 10) {
        Operation op = p.createOperation("dfs-vertices", V);
        Graph* g = createGraph(V);
        generateRandomGraph(g, fixedE);
        
        dfs(g, false, &op, NULL); 
        
        freeGraph(g);
    }
    
    p.createGroup("DFS_Edges", "dfs-edges");
    p.createGroup("DFS_Vertices", "dfs-vertices");

    p.showReport();
}

int main() {
    demo();
    // perf();
    return 0;
}
