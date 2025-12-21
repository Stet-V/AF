#include <stdlib.h>
#include <string.h>
#include "bfs.h"

/*
    Stet Vasile-Alexandru, Grupa 10

    Am implementat algoritmul BFS (Breadth-First Search) pentru parcurgerea grafurilor reprezentate sub forma de grila,
    utilizand structuri de date pentru noduri si grafuri. Am inclus si functii auxiliare pentru afisarea arborelui BFS,
    calculul drumului minim si generarea grafurilor conexe pentru testare de performanta.
    De asemenea, am implementat o functie de performanta pentru masurarea complexitatii in functie de numarul de muchii si varfuri.

    1. get_neighbors(grid, p, neighb):
    Returneaza vecinii valizi (sus, jos, stanga, dreapta) ai unui punct p in grila.
    Complexitate: O(1).

    2. bfs(graph, s, op):
    Initializeaza nodurile (culoare WHITE, dist -1, parent NULL).
    Foloseste o coada pentru parcurgere BFS de la sursa s.
    Actualizeaza distante, parinti si culori pentru vecini nevizitati.
    Complexitate: O(V + E).

    3. print_node_recursive(node, all_nodes, nrNodes, indent_spaces):
    Afiseaza recursiv nodul si copiii sai cu indentare.
    Folosit in print_bfs_tree pentru afisarea arborelui BFS.
    Complexitate: O(V).

    4. shortest_path(graph, start, end, path):
    Ruleaza BFS de la start, apoi reconstruieste drumul de la end la start folosind parintii.
    Returneaza lungimea drumului sau -1 daca inaccesibil.
    Complexitate: O(V + E).

    5. generate_conn_graph(graph, n, nrEdges):
    Genereaza un graf conex aleator cu n noduri si nrEdges muchii folosind o matrice de adiacenta temporara.
    Asigura conexitate prin constructie de arbore si adauga muchii extra.
    Complexitate: O(V^2).

    6. performance():
    Testeaza performanta BFS variind numarul de muchii (fix V=100) si numarul de varfuri (fix E=4950).

    Spatiu total: O(V + E) pentru graf si structuri auxiliare.
*/

int get_neighbors(const Grid *grid, Point p, Point neighb[]) {
    int count = 0;
    int rowDir[] = {-1, 1, 0, 0};
    int colDir[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int newRow = p.row + rowDir[i];
        int newCol = p.col + colDir[i];

        if (newRow >= 0 && newRow < grid->rows && newCol >= 0 && newCol < grid->cols) {
            if (grid->mat[newRow][newCol] == 0) {
                neighb[count].row = newRow;
                neighb[count].col = newCol;
                count++;
            }
        }
    }

    return count;
}

void grid_to_graph(const Grid *grid, Graph *graph) {
    Node *nodes[MAX_ROWS][MAX_COLS];
    int i, j, k;
    Point neighb[4];

    graph->nrNodes = 0;
    for (i = 0; i < grid->rows; ++i) {
        for (j = 0; j < grid->cols; ++j) {
            if (grid->mat[i][j] == 0) {
                nodes[i][j] = (Node *)malloc(sizeof(Node));
                memset(nodes[i][j], 0, sizeof(Node));
                nodes[i][j]->position.row = i;
                nodes[i][j]->position.col = j;
                ++graph->nrNodes;
            } else {
                nodes[i][j] = NULL;
            }
        }
    }

    graph->v = (Node **)malloc(graph->nrNodes * sizeof(Node *));
    k = 0;
    for (i = 0; i < grid->rows; ++i) {
        for (j = 0; j < grid->cols; ++j) {
            if (nodes[i][j] != NULL) {
                graph->v[k++] = nodes[i][j];
            }
        }
    }

    for (i = 0; i < graph->nrNodes; ++i) {
        graph->v[i]->adjSize = get_neighbors(grid, graph->v[i]->position, neighb);
        if (graph->v[i]->adjSize != 0) {
            graph->v[i]->adj = (Node **)malloc(graph->v[i]->adjSize * sizeof(Node *));
            k = 0;
            for (j = 0; j < graph->v[i]->adjSize; ++j) {
                if (neighb[j].row >= 0 && neighb[j].row < grid->rows &&
                    neighb[j].col >= 0 && neighb[j].col < grid->cols &&
                    grid->mat[neighb[j].row][neighb[j].col] == 0) {
                    graph->v[i]->adj[k++] = nodes[neighb[j].row][neighb[j].col];
                }
            }
            if (k < graph->v[i]->adjSize) {
                graph->v[i]->adjSize = k;
                graph->v[i]->adj = (Node **)realloc(graph->v[i]->adj, k * sizeof(Node *));
            }
        }
    }
}

void free_graph(Graph *graph) {
    if (graph->v != NULL) {
        for (int i = 0; i < graph->nrNodes; ++i) {
            if (graph->v[i] != NULL) {
                if (graph->v[i]->adj != NULL) {
                    free(graph->v[i]->adj);
                    graph->v[i]->adj = NULL;
                }
                graph->v[i]->adjSize = 0;
                free(graph->v[i]);
                graph->v[i] = NULL;
            }
        }
        free(graph->v);
        graph->v = NULL;
    }
    graph->nrNodes = 0;
}

void bfs(Graph *graph, Node *s, Operation *op) {
    for (int i = 0; i < graph->nrNodes; ++i) {
        if (op != NULL) {
            op->count();
        }
        graph->v[i]->color = COLOR_WHITE;
        graph->v[i]->dist = -1;
        graph->v[i]->parent = NULL;
    }

    Node **Q = (Node **)malloc(graph->nrNodes * sizeof(Node *));
    int head = 0;
    int tail = 0;

    s->color = COLOR_GRAY;
    s->dist = 0;
    s->parent = NULL;
    Q[tail++] = s;
    if (op != NULL) {
        op->count();
    }

    while (head != tail) {
        if (op != NULL) {
            op->count();
        }
        Node *u = Q[head++];

        for (int i = 0; i < u->adjSize; ++i) {
            if (op != NULL) {
                op->count();
            }
            Node *v = u->adj[i];

            if (v->color == COLOR_WHITE) {
                v->color = COLOR_GRAY;
                v->dist = u->dist + 1;
                v->parent = u;
                Q[tail++] = v;
                if (op != NULL) {
                    op->count();
                }
            }
        }
        u->color = COLOR_BLACK;
    }

    free(Q);
}

void print_node_recursive(Node *node, Node **all_nodes, int nrNodes, int indent_spaces) {
    if (node == NULL || node->color != COLOR_BLACK) {
        return;
    }

    for (int i = 0; i < indent_spaces * 4; i++) {
        printf("  ");
    }
    printf("(%d, %d)\n", node->position.row, node->position.col);

    int new_indent = indent_spaces + 1;
    for (int i = 0; i < nrNodes; ++i) {
        if (all_nodes[i]->parent == node) {
            print_node_recursive(all_nodes[i], all_nodes, nrNodes, new_indent);
        }
    }
}

void print_bfs_tree(Graph *graph) {
    Node *root = NULL;

    for (int i = 0; i < graph->nrNodes; ++i) {
        if (graph->v[i]->dist == 0 && graph->v[i]->parent == NULL && graph->v[i]->color == COLOR_BLACK) {
            root = graph->v[i];
            break;
        }
    }

    if (root == NULL) {
        return;
    }

    print_node_recursive(root, graph->v, graph->nrNodes, 0);
}

int shortest_path(Graph *graph, Node *start, Node *end, Node *path[]) {
    bfs(graph, start, NULL);

    if (end->dist == -1) {
        return -1;
    }

    int length = 0;
    Node *curr = end;

    while (curr != NULL) {
        length++;
        curr = curr->parent;
    }

    curr = end;
    int index = length - 1;
    while (curr != NULL) {
        path[index] = curr;
        index--;
        curr = curr->parent;
    }

    return length;
}

void generate_conn_graph(Graph *graph, int n, int nrEdges) {
    int **adjMat = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; ++i) {
        adjMat[i] = (int *)calloc(n, sizeof(int));
    }

    int edges_count = 0;

    for (int i = 1; i < n; ++i) {
        int parent = rand() % i;
        adjMat[i][parent] = 1;
        adjMat[parent][i] = 1;
        edges_count++;
    }

    while (edges_count < nrEdges) {
        int u = rand() % n;
        int v = rand() % n;

        if (u != v && adjMat[u][v] == 0) {
            adjMat[u][v] = 1;
            adjMat[v][u] = 1;
            edges_count++;
        }
    }

    for (int i = 0; i < n; ++i) {
        int count = 0;
        for (int j = 0; j < n; ++j) {
            if (adjMat[i][j] == 1) {
                count++;
            }
        }

        graph->v[i]->adjSize = count;
        if (count > 0) {
            graph->v[i]->adj = (Node **)malloc(count * sizeof(Node *));
            int idx = 0;
            for (int j = 0; j < n; ++j) {
                if (adjMat[i][j] == 1) {
                    graph->v[i]->adj[idx++] = graph->v[j];
                }
            }
        } else {
            graph->v[i]->adj = NULL;
        }

        graph->v[i]->position.row = 0;
        graph->v[i]->position.col = 0;
    }

    for (int i = 0; i < n; ++i) {
        free(adjMat[i]);
    }
    free(adjMat);
}

void performance() {
    int n, i;
    Profiler p("bfs");

    printf("Running BFS performance for variable edges...\n");
    for (n = 100; n <= 4950; n += 100) {
        Operation op = p.createOperation("bfs-edges", n);
        Graph graph;
        graph.nrNodes = 100;

        graph.v = (Node **)malloc(graph.nrNodes * sizeof(Node *));
        for (i = 0; i < graph.nrNodes; ++i) {
            graph.v[i] = (Node *)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
        }

        generate_conn_graph(&graph, graph.nrNodes, n);

        bfs(&graph, graph.v[0], &op);

        free_graph(&graph);
    }

    printf("Running BFS performance for variable vertices...\n");
    for (n = 100; n <= 10000; n += 100) {
        Operation op = p.createOperation("bfs-vertices", n);
        Graph graph;
        graph.nrNodes = n;

        graph.v = (Node **)malloc(graph.nrNodes * sizeof(Node *));
        for (i = 0; i < graph.nrNodes; ++i) {
            graph.v[i] = (Node *)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
        }

        generate_conn_graph(&graph, n, 4950);

        bfs(&graph, graph.v[0], &op);

        free_graph(&graph);
    }

    p.showReport();
}
