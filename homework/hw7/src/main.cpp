/*
    Stet Vasile-Alexandru, Grupa 10

    Am implementat un arbore binar de cautare echilibrat (AVL) augmentat cu informatii de dimensiune a subarborilor (size) 
    pentru a suporta operatii de statistica de ordin: OS_Select (gasirea nodului de rang i) si stergerea unui nod 
    identificat prin rang, mentinand in acelasi timp echilibrul AVL.

    Structura nodului (BSTNode) contine:
    - key: cheia nodului
    - size: dimensiunea subarborelui (numar de noduri)
    - height: inaltimea subarborelui
    - left, right, parent: pointeri clasici + pointer catre parinte (esential pentru reechilibrarea la stergere)

    Operatii principale:
    1. buildTree(n): construieste recursiv un arbore perfect echilibrat cu cheile 1..n (metoda divide-et-impera: 
       mijlocul devine radacina, se construiesc recursiv subarborii stang si drept). Complexitate O(n) timp, 
       rezulta un arbore de inaltime floor(log2(n)) + 1.
    2. OS_Select(node, i): returneaza nodul cu rangul i in parcurgerea inordine (1 = cel mai mic). 
       Foloseste campul size pentru decizii rapide. Complexitate O(log n).
    3. OS_Delete(root, node): sterge nodul dat, gestionand toate cazurile:
       - 0 sau 1 fiu -> inlocuire directa
       - 2 fii -> inlocuirea cheii cu succesorul (cel mai mic din subarborele drept), apoi stergere recursiva a succesorului
       Dupa stergere, se reechilibreaza AVL pornind de la parintele nodului fizic eliminat pana la radacina:
       - actualizare metrics (height + size)
       - verificare factor de echilibru
       - rotatii simple (LL/RR) sau duble (LR/RL) daca |balance| > 1
       Toate legaturile parent sunt mentinute corect in rotatii si inlocuiri.
       Complexitate O(log n).

    Complexitati:
    - buildTree:               O(n) timp, O(n) spatiu auxiliar (stiva recursiva O(log n))
    - OS_Select:               O(log n) timp
    - OS_Delete:               O(log n) timp (cautarea succesorului + drumul in sus pentru reechilibrare)
    - Spatiu total:            O(n) pentru arbore + pointeri parent
    - prettyPrint:             O(n) timp
*/

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Profiler.h"

#define MAX_N 10000
#define NR_TESTS 5

Profiler p;

typedef struct BSTNode {
    int key, size, height;
    BSTNode* left;
    BSTNode* right;
    BSTNode* parent;
} BSTNode;

BSTNode* createBSTNode(int key) {
    BSTNode* newNode = new BSTNode;

    newNode->key = key;
    newNode->size = 1; 
    newNode->height = 1;

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;

    return newNode;
}

int getHeight(BSTNode* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

int getSize(BSTNode* node) {
    if (node == NULL) {
        return 0;
    }
    return node->size;
}

int getBalance(BSTNode* node) {
    if (node == NULL) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

void updateMetrics(BSTNode* node, long long& count) {
    count += 1;
    if (node == NULL) {
        return;
    }
    int left_height = getHeight(node->left);
    int right_height = getHeight(node->right);

    count += 2;
    node->height = 1 + (left_height > right_height ? left_height : right_height);
    
    count += 1;
    node->size = 1 + getSize(node->left) + getSize(node->right);
}

BSTNode* rightRotate(BSTNode* y, long long& count) {
    count += 2;
    BSTNode* x = y->left;
    BSTNode* T2 = x->right;

    count += 4;
    x->right = y;
    y->left = T2;
    x->parent = y->parent;
    y->parent = x;
    
    count += 1;
    if (T2 != NULL) {
        count += 1;
        T2->parent = y;
    }
    
    updateMetrics(y, count);
    updateMetrics(x, count);

    return x;
}

BSTNode* leftRotate(BSTNode* x, long long& count) {
    count += 2;
    BSTNode* y = x->right;
    BSTNode* T2 = y->left;

    count += 4;
    y->left = x;
    x->right = T2;
    y->parent = x->parent;
    x->parent = y;

    count += 1;
    if (T2 != NULL) {
        count += 1;
        T2->parent = x;
    }

    updateMetrics(x, count);
    updateMetrics(y, count);

    return y;
}

BSTNode* buildTreeRecursive(int start, int end, BSTNode* parent, long long& count) {
    count += 1;
    if (start > end) {
        return NULL;
    }

    count += 2;
    int mid = start + (end - start) / 2;

    count += 7;
    BSTNode* node = createBSTNode(mid);
    
    count += 3;
    node->parent = parent;
    node->left = buildTreeRecursive(start, mid - 1, node, count);
    node->right = buildTreeRecursive(mid + 1, end, node, count);

    updateMetrics(node, count);
    
    return node;
}

BSTNode* buildTree(int n, long long& count) {
    return buildTreeRecursive(1, n, NULL, count);
}

void freeTree(BSTNode* node) {
    if (node == NULL) {
        return;
    }
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

BSTNode* OS_Select(BSTNode* node, int i, long long& count) {
    count += 1;
    if (node == NULL) {
        return NULL;
    }

    count += 1;
    int left_size = getSize(node->left); 

    count += 1;
    if (i == left_size + 1) {
        return node;
    }
    
    count += 1;
    if (i <= left_size) {
        return OS_Select(node->left, i, count);
    } else {
        return OS_Select(node->right, i - left_size - 1, count);
    }
}

BSTNode* OS_Delete(BSTNode* root, BSTNode* node, long long& count) {
    count += 1;
    if (node == NULL) {
        return root;
    }

    BSTNode* parent_to_update = NULL;
    
    count += 2;
    if (node->left == NULL || node->right == NULL) {
        count += 2;
        BSTNode* temp = (node->left != NULL) ? node->left : node->right;
        count += 1;
        parent_to_update = node->parent;

        count += 1;
        if (node->parent == NULL) {
            count += 1;
            root = temp;
        } else {
            count += 1;
            if (node->parent->left == node) {
                count += 1;
                node->parent->left = temp;
            } else {
                count += 1;
                node->parent->right = temp;
            }
        }

        count += 1;
        if (temp) {
            count += 1;
            temp->parent = node->parent;
        }

        delete node;
    } 
    else {
        count += 1;
        BSTNode* successor = node->right;
        count += 1;
        while (successor->left != NULL) {
            count += 2;
            successor = successor->left;
        }

        count += 1;
        node->key = successor->key;
        root = OS_Delete(root, successor, count);
        parent_to_update = NULL;
    }

    count += 1;
    if (parent_to_update != NULL) {
        count += 1;
        BSTNode* current = parent_to_update;
        count += 1;
        while (current != NULL) {
            count += 1;
            BSTNode* parent_of_current = current->parent;
            
            updateMetrics(current, count);

            count += 2;
            int balance = getBalance(current);
            
            BSTNode* new_subtree_root = current;

            count += 1;
            if (balance > 1) {
                count += 3;
                if (getBalance(current->left) >= 0) {
                    count += 1;
                    new_subtree_root = rightRotate(current, count);
                } else {
                    count += 1;
                    current->left = leftRotate(current->left, count);
                    new_subtree_root = rightRotate(current, count);
                }
            } else {
                count += 1;
                if (balance < -1) {
                    count += 3;
                    if (getBalance(current->right) <= 0) {
                        count += 1;
                        new_subtree_root = leftRotate(current, count);
                    } else {
                        count += 1;
                        current->right = rightRotate(current->right, count);
                        new_subtree_root = leftRotate(current, count);
                    }
                }
            }

            count += 1;
            if (parent_of_current == NULL) {
                count += 1;
                root = new_subtree_root;
            } else {
                count += 1;
                if (parent_of_current->left == current) {
                    count += 1;
                    parent_of_current->left = new_subtree_root;
                } else {
                    count += 1;
                    parent_of_current->right = new_subtree_root;
                }
            }

            count += 1;
            if (new_subtree_root != NULL) {
                count += 1;
                new_subtree_root->parent = parent_of_current;
            }
            
            count += 1;
            current = parent_of_current;
        }
    }

    return root;
}

void prettyPrint(BSTNode* root, int spaces) {
    if (root == NULL) {
        return;
    }
    for (int i = 0; i < spaces; i++) {
        printf("  ");
    }
    printf("%d (s=%d, h=%d)\n", root->key, root->size, root->height);
    prettyPrint(root->left, spaces + 1);
    prettyPrint(root->right, spaces + 1);
}

void demo() {
    long long dummyCount = 0;
    int n = 8;
    BSTNode* root = buildTree(n, dummyCount);
    prettyPrint(root, 0);

    for (int i = 0; i < 3; i++) {
        int rank = 1 + rand() % n;
        BSTNode* selectedNode = OS_Select(root, rank, dummyCount);
        if (selectedNode) {
            printf("Rank: %d, Key: %d.\n", rank, selectedNode->key);
        } else {
            printf("Not found.\n");
        }
    }
    
    for (int i = 0; i < 3; i++) {
        int rank = 1 + rand() % n;
        // if (i == 0) {
        //     rank = 5;
        // } 
        BSTNode* selectedNode = OS_Select(root, rank, dummyCount);
        if (selectedNode) {
            printf("Rank: %d, Key: %d.\n", rank, selectedNode->key);
            root = OS_Delete(root, selectedNode, dummyCount);
            n--;
        } else {
            printf("Not found.\n", rank);
        }
        prettyPrint(root, 0);
    }
    
    freeTree(root);
}

void perf() {
    for (int n = 100; n <= MAX_N; n += 100) {

        Operation opBuild = p.createOperation("Build Tree", n);
        Operation opSelect = p.createOperation("OS_Select", n);
        Operation opDelete = p.createOperation("OS_Delete", n);

        long long buildCount = 0;
        long long selectCount = 0;
        long long deleteCount = 0;

        for (int i = 0; i < NR_TESTS; i++) {
            BSTNode* root = buildTree(n, buildCount);
            for (int j = 0; j < n; j++) {
                int rank = 1 + rand() % (n - j);
                BSTNode* selectedNode = OS_Select(root, rank, selectCount);
                if (selectedNode) {
                    root = OS_Delete(root, selectedNode, deleteCount);
                }
            }
            freeTree(root);
        }

        opBuild.count(buildCount / NR_TESTS);
        opSelect.count(selectCount / NR_TESTS);
        opDelete.count(deleteCount / NR_TESTS);
    }
    p.createGroup("Operations Count", "Build Tree", "OS_Select", "OS_Delete");

    p.showReport();
}

int main() {
    srand(time(NULL));
//    demo();
    perf();
    return 0;
}