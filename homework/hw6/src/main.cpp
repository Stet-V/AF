/*
    Stet Vasile-Alexandru, Grupa 10

    Am implementat operatii pentru arbori cu cai multiple, incluzand transformari intre diverse reprezentari (vector de parinti 
    R1, lista de fii R2, binar R3) si parcurgeri ale arborelui binar.

    Trei reprezentari de arbore sunt gestionate:
    1.  R1 (parent[]): Vector de parinti, unde `parent[i]` este indexul parintelui nodului `i+1`. Radacina este marcata cu -1.
    2.  R2 (NodeR2): Structura cu cheie si un tablou de pointeri la fii (`children[]`).
    3.  R3 (NodeR3): Structura de arbore binar, unde `left` indica primul fiu, iar `right` indica urmatorul frate (reprezentare fiu-frate stanga).

    Sunt implementate functii de pretty-print pentru cele trei reprezentari (R1, R2, R3) si cele doua transformari esentiale:
    T1 (R1 -> R2): Transforma reprezentarea cu vector de parinti in reprezentarea cu lista de fii. Utilizeaza un tablou auxiliar (`nodes`) pentru 
    acces rapid O(1) la noduri.
    T2 (R2 -> R3): Transforma arborele cu cai multiple (R2) in reprezentarea binara (R3) folosind metoda fiu-frate stanga. Se bazeaza pe o 
    parcurgere recursiva, unde primul fiu este legat la `left`, iar fratii sunt legati la `right`.

    De asemenea, am inclus parcurgerea Inordine (recursiva si iterativa) pentru arborele binar (R3), folosind pentru cea iterativa algoritmul 
    Morris Traversal pentru a obtine o complexitate de memorie O(1) suplimentara.

    Complexitati:
    Timp PrettyPrint R1: O(n^2) in caz defavorabil (arbore degenerat), deoarece cautarea copiilor in vectorul `parents` la fiecare nivel 
    necesita O(n).
    Timp PrettyPrint R2, R3: O(n), deoarece fiecare nod este vizitat o singura data.
    Timp T1 (R1 -> R2): O(n), necesita doua parcurgeri ale vectorului `parent` si utilizarea tabloului auxiliar O(n) pentru a asigura inserarea in
    O(1) a copiilor.
    Timp T2 (R2 -> R3): O(n), deoarece fiecare nod este vizitat o singura data in recursivitate.
    Timp Parcurgeri Inordine (R3): Atat cea recursiva, cat si cea iterativa (Morris Traversal) au complexitatea O(n).
    Spatiu: O(n) pentru toate reprezentarile si transformarile (cu exceptia parcurgerii Inordine iterative cu O(1) spatiu suplimentar, 
    ignorand arborele).

    Observatii:
    Morris Traversal reuseste parcurgerea inordine iterativa a arborelui binar in O(n) timp si O(1) spatiu suplimentar modificand temporar 
    pointerii `right` (pentru a indica predecesorul inordine) si restaurandu-i ulterior.
*/

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Profiler.h"

Profiler p("Inorder Traversal Operations");

#define MAX_CHILDREN 100
#define MAX_N 10000

typedef struct NodeR2 {
    int key;
    struct NodeR2* children[MAX_CHILDREN] = {};
    int children_count;
} NodeR2;

typedef struct NodeR3 {
    int key;
    struct NodeR3* left;
    struct NodeR3* right;
} NodeR3;

NodeR2* createNodeR2(int key) {
    NodeR2* newNode = new NodeR2;
    newNode->key = key;
    newNode->children_count = 0;
    return newNode;
}

NodeR3* createNodeR3(int key) {
    NodeR3* newNode = new NodeR3;
    newNode->key = key;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void freeR2Tree(NodeR2* node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < node->children_count; i++) {
        freeR2Tree(node->children[i]);
    }
    free(node);
}

void freeR3Tree(NodeR3* node) {
    if (node == NULL) {
        return;
    }
    freeR3Tree(node->left);
    freeR3Tree(node->right);
    free(node);
}

NodeR3* generateBinaryTree(int n) {
    if (n <= 0) {
        return NULL;
    }
    NodeR3* root = createNodeR3(rand() % 100);
    int left_subtree_size = rand() % n;
    root->left = generateBinaryTree(left_subtree_size);
    root->right = generateBinaryTree(n - 1 - left_subtree_size);
    return root;
}

void recursiveInorder(NodeR3* node, Operation operation_counter, boolean print) {
    if (node == NULL) {
        return;
    }
    operation_counter.count();
    recursiveInorder(node->left, operation_counter, print);
    if (print) {
        printf("%d ", node->key);
    }
    recursiveInorder(node->right, operation_counter, print);
}

void iterativeInorder(NodeR3* root, Operation operation_counter, boolean print) {
    NodeR3* current = root;
    NodeR3* predecessor = NULL;

    while (current != NULL) {
        if (current->left == NULL) {
            operation_counter.count();
            if (print) {
                printf("%d ", current->key);
            }
            current = current->right;
        } else {
            predecessor = current->left;
            while (predecessor->right != NULL && predecessor->right != current) {
                predecessor = predecessor->right;
            }
            if (predecessor->right == NULL) {
                predecessor->right = current;
                current = current->left;
            } else {
                predecessor->right = NULL;
                operation_counter.count();
                if (print) {
                    printf("%d ", current->key);
                }
                current = current->right;
            }
        }
    }
}

void prettyPrintR1(int parents[], int n, int indexParent, int indent_spaces) {
    for (int i = 0; i < n; i++) {
        if (parents[i] == indexParent) {
            for (int j = 0; j < indent_spaces; j++) {
                printf("  ");
            }
            printf("%d\n", i + 1);
            prettyPrintR1(parents, n, i + 1, indent_spaces + 1);
        }
    }
}

void prettyPrintR2(NodeR2 *R2, int indent_spaces) {
    if (R2 == NULL) {
        return;
    }
    for(int i = 0; i < indent_spaces; i++) {
        printf("  ");
    }
    printf("%d\n", R2->key);
    for(int i = 0; i < R2->children_count; i++) {
        prettyPrintR2(R2->children[i], indent_spaces + 1);
    }
}

void prettyPrintR3(NodeR3 *R3, int indent_spaces) {
    if (R3 == NULL) {
        return;
    }
    for (int i = 0; i < indent_spaces; i++) {
        printf("  ");
    }
    printf("%d\n", R3->key);
    prettyPrintR3(R3->left, indent_spaces + 1);
    prettyPrintR3(R3->right, indent_spaces);
}

NodeR2* T1(int parent[], int size) {
    NodeR2* root = NULL;
    NodeR2** nodes = new NodeR2*[size];

    for (int i = 0; i < size; i++) {    // O(n)
        nodes[i] = createNodeR2(i + 1);
        if (!nodes[i]) {
            for (int j = 0; j < i; j++) {
                free(nodes[j]);
            }
            free(nodes);
            return NULL;
        }
    }

    for (int i = 0; i < size; i++) {    // O(n)
        if (parent[i] == -1) {
            root = nodes[i];
        } else {
            int parent_index = parent[i] - 1;
            if (parent_index >= 0 && parent_index < size) {
                NodeR2* parent_node = nodes[parent_index];
                NodeR2* child_node = nodes[i];

                if (parent_node->children_count < MAX_CHILDREN) {
                    parent_node->children[parent_node->children_count] = child_node;
                    parent_node->children_count++;
                }
            }
        }
    }
    free(nodes);
    return root;
}

NodeR3* T2(NodeR2* root) {
    if (root == NULL) {
        return NULL;
    }

    NodeR3* newNode = createNodeR3(root->key);
    if (newNode == NULL) {
        return NULL;
    }

    if (root->children_count > 0) {
        newNode->left = T2(root->children[0]);

        NodeR3* current_sibling = newNode->left;
        
        for (int i = 1; i < root->children_count; i++) {
            if (current_sibling != NULL) {
                current_sibling->right = T2(root->children[i]); // O(n)
                current_sibling = current_sibling->right;
            }
        }
    }
    
    return newNode;
}

void demo() {
    int current_key = 1;
    Operation opDummy = p.createOperation("Dummy", 1); // Pentru a evita erori Profiler la crearea de operatii
    NodeR3* binary_tree_root = generateBinaryTree(10);
    printf("Recursive Inorder:\n");
    recursiveInorder(binary_tree_root, opDummy, true);

    printf("\nIterative Inorder:\n");
    iterativeInorder(binary_tree_root, opDummy, true);
    printf("\n");

    int pi[] = {2, 7, 5, 2, 7, 7, -1, 5, 2};
    int size_pi = sizeof(pi) / sizeof(pi[0]);
    
    printf("R1:\n");
    prettyPrintR1(pi, size_pi, -1, 0); 
    
    printf("R2 (after T1):\n");
    NodeR2* R2_root = T1(pi, size_pi);
    prettyPrintR2(R2_root, 0);

    NodeR3* R3_root = T2(R2_root);
    printf("R3 (after T2)\n");
    prettyPrintR3(R3_root, 0);

    freeR2Tree(R2_root);
    freeR3Tree(R3_root);
    freeR3Tree(binary_tree_root);
}

void perf() {
    for (int n = 100; n <= MAX_N; n += 100) {
        Operation opRecursive = p.createOperation("Inorder Recursive", n);
        Operation opIterative = p.createOperation("Inorder Iterative", n);
        int current_key = 1;
        NodeR3* binary_tree_root = generateBinaryTree(n);
        recursiveInorder(binary_tree_root, opRecursive, false);
        iterativeInorder(binary_tree_root, opIterative, false);
        freeR3Tree(binary_tree_root);
    }
    p.createGroup("Inorder Traversal Operations", "Inorder Recursive", "Inorder Iterative");
    p.showReport();
}

int main() {
    srand(time(NULL));
    demo();
    // perf();
    return 0;
}