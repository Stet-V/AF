/*
    Stet Vasile-Alexandru, Grupa 10

    Am implementat un tablou de dispersie folosind sondare cuadratica pentru rezolvarea coliziunilor, cu suport pentru operatii de inserare, cautare si
    stergere, precum si un profiler care genereaza un raport HTML pentru analiza performantei.

    Tabloul de dispersie foloseste un vector de structuri Entry, fiecare continand un id si un nume. Functia de dispersie primara este 
    id % table_size, iar sondarea cuadratica se calculeaza astfel: (h_prime + C1 * i * i + C2 * i + C3) % table_size. Inserarea reutilizeaza
    sloturile sterse daca este posibil, stergerea marcheaza intrarile cu DELETED_ID, iar cautarea se opreste la EMPTY_ID.

    Functia perf masoara eforturile medii si maxime de sondare pentru cautari reusite si nereusite, la factori de incarcare variati (alpha de la 0.80
    la 0.99), inainte si dupa stergeri. Rezultatele reprezinta media peste NR_TESTS = 5 rulari si sunt compilate intr-un tabel.

    Complexitati:
    Timp: In medie O(1) pentru insert/search/delete; in caz defavorabil O(n);
    Spatiu: O(N) pentru tabela.

    Observatii:
    Eforturile cresc odata cu factorul de incarcare alpha, mai accentuat aproape de 1.0. Dupa stergeri, eforturile pentru cautari 
    cresc in continuare datorita elementelor sterse.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "Profiler.h"
#include "table_profiler.h"

#define C1 1
#define C2 1
#define C3 0
#define EMPTY_ID -1
#define DELETED_ID -2
#define N 10007
#define NR_TESTS 5
#define MIN_INSERT_VAL 1
#define MAX_INSERT_VAL 50000
#define MIN_NOTFOUND_VAL ((MAX_INSERT_VAL) + (MIN_INSERT_VAL))
#define MAX_NOTFOUND_VAL (2 * (MAX_INSERT_VAL))

typedef struct Entry {
    int id;
    char name[30];
} Entry;

void shuffle(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

int hash_prime(int id, int table_size) {
    return id % table_size;
}

int quadratic_probe(int h_prime, int i, int table_size) {
    return (h_prime + C1 * i * i + C2 * i + C3) % table_size;
}

void hash_insert(Entry hashTable[], int table_size, Entry entry, bool printSteps) {
    int h_prime = hash_prime(entry.id, table_size);
    int h;
    int first_deleted = -1;

    for (int i = 0; i < table_size; i++) {
        h = quadratic_probe(h_prime, i, table_size);

        if (hashTable[h].id == EMPTY_ID) {
            int insert_pos = (first_deleted != -1) ? first_deleted : h;
            hashTable[insert_pos] = entry;

            if (printSteps) {
                if (insert_pos == h) {
                    printf("Inserted (%d, '%s') at position %d (probe i=%d)\n", entry.id, entry.name, insert_pos, i);
                } else {
                    printf("Inserted (%d, '%s') at position %d (re-using deleted slot, probe i=%d)\n", entry.id, entry.name, insert_pos, i);
                }
            }
            return;
        }
        if (hashTable[h].id == entry.id) {
            if (printSteps) {
                printf("ID %d already exists at position %d.\n", entry.id, h);
            }
            return;
        }

        if (hashTable[h].id == DELETED_ID) {
            if (first_deleted == -1) {
                first_deleted = h;
            }
        }
    }
    if (first_deleted != -1) {
        hashTable[first_deleted] = entry;
        if (printSteps) {
            printf("Inserted (%d, '%s') at position %d (re-using deleted slot, table is full).\n", entry.id, entry.name, first_deleted);
        }
        return;
    }
    if (printSteps) {
        printf("Insertion failed for (%d, '%s') - Table is full.\n", entry.id, entry.name);
    }
}

Entry* hash_search(Entry hashTable[], int table_size, int id, int& operations) {
    int h_prime = hash_prime(id, table_size);
    int h;
    operations = 0;

    for (int i = 0; i < table_size; i++) {
        h = quadratic_probe(h_prime, i, table_size);
        operations++;

        if (hashTable[h].id == EMPTY_ID) {
            return nullptr;
        }
        if (hashTable[h].id == id) {
            return &hashTable[h];
        }
    }
    return nullptr;
}

bool hash_delete(Entry hashTable[], int table_size, int id, bool printSteps) {
    int h_prime = hash_prime(id, table_size);
    int h;

    for (int i = 0; i < table_size; i++) {
        h = quadratic_probe(h_prime, i, table_size);

        if (hashTable[h].id == EMPTY_ID) {
            if (printSteps) {
                printf("ID %d not found (hit EMPTY at %d).\n", id, h);
            }
            return false;
        }

        if (hashTable[h].id == id) {
            hashTable[h].id = DELETED_ID;
            hashTable[h].name[0] = '\0';
            if (printSteps) {
                printf("Deleted ID %d from position %d.\n", id, h);
            }
            return true;
        }
    }
    if (printSteps) {
        printf("ID %d not found (full scan).\n", id);
    }
    return false;
}

void printHashTable(Entry hashTable[], int table_size) {
    for (int i = 0; i < table_size; i++) {
        if (hashTable[i].id == EMPTY_ID) {
            printf("Index %d: [EMPTY]\n", i);
        } else if (hashTable[i].id == DELETED_ID) {
            printf("Index %d: [DELETED]\n", i);
        } else {
            printf("Index %d: (%d, '%s')\n", i, hashTable[i].id, hashTable[i].name);
        }
    }
}

void demo() {
    int a[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(a)/sizeof(a[0]);

    int table_size = 10;
    Entry* hashTable = new Entry[table_size];

    for (int i = 0; i < table_size; i++) {
        hashTable[i].id = EMPTY_ID;
        hashTable[i].name[0] = '\0';
    }

    printf("Inserting entries:\n");
    for (int i = 0; i < n; i++) {
        Entry entry;
        entry.id = a[i];
        snprintf(entry.name, sizeof(entry.name), "Name%d", a[i]);
        hash_insert(hashTable, table_size, entry, true);
    }

    printf("Contents:\n");
    printHashTable(hashTable, table_size);

    int b[] = {34, 100, 12};
    int m = sizeof(b)/sizeof(b[0]);
    printf("Deleting entries:\n");
    for (int i = 0; i < m; i++) {
        hash_delete(hashTable, table_size, b[i], true);
    }

    int c[] = {25, 34, 54, 22};
    int l = sizeof(c)/sizeof(c[0]);
    printf("Searching entries:\n");
    for (int i = 0; i < l; i++) {
        int dummy_ops;
        Entry* result = hash_search(hashTable, table_size, c[i], dummy_ops);
        if (result != nullptr) {
            printf("Found ID %d: Name='%s'\n", result->id, result->name);
        } else {
            printf("ID %d not found.\n", c[i]);
        }
    }

    delete[] hashTable;
}

void perfBeforeDeletion(const char* filename) {
    int* a = new int[N];
    float alpha_values[] = {0.80, 0.85, 0.90, 0.95, 0.99};
    int num_alphas = sizeof(alpha_values) / sizeof(alpha_values[0]);

    double data[num_alphas][5];

    for (int i = 0; i < num_alphas; i++) {
        float alpha = alpha_values[i];
        
        double total_avg_ops_found = 0;
        double total_avg_ops_not_found = 0;

        int total_max_ops_found = 0;
        int total_max_ops_not_found = 0;

        for (int test = 0; test < NR_TESTS; test++) {
            int n = (int)(alpha * N);
            Entry* hashTable = new Entry[N];

            for (int j = 0; j < N; j++) {
                hashTable[j].id = EMPTY_ID;
                hashTable[j].name[0] = '\0';
            }

            FillRandomArray(a, n, MIN_INSERT_VAL, MAX_INSERT_VAL, true, UNSORTED);

            for (int j = 0; j < n; j++) {
                Entry entry;
                entry.id = a[j];
                hash_insert(hashTable, N, entry, false);
            }

            const int m = 3000;
            const int m_found = 1500;
            const int m_not_found = m - m_found;
            int* search_keys = new int[m];

            for (int j = 0; j < m_found; j++) {
                search_keys[j] = a[rand() % n];
            }

            for (int j = m_found; j < m; j++) {
                search_keys[j] = MIN_NOTFOUND_VAL + (rand() % (MAX_NOTFOUND_VAL - MIN_NOTFOUND_VAL + 1));
            }

            shuffle(search_keys, m);

            long long total_ops_found = 0;
            long long total_ops_not_found = 0;

            int count_found = 0;
            int count_not_found = 0;

            int max_ops_found = 0;
            int max_ops_not_found = 0;

            for (int j = 0; j < m; j++) {
                int ops = 0;
                Entry* result = hash_search(hashTable, N, search_keys[j], ops);

                if (result != nullptr) {
                    total_ops_found += ops;
                    count_found++;
                    if (ops > max_ops_found) {
                        max_ops_found = ops;
                    }
                } else {
                    total_ops_not_found += ops;
                    count_not_found++;
                    if (ops > max_ops_not_found) {
                        max_ops_not_found = ops;
                    }
                }
            }

            double avg_ops_found = (count_found > 0) ? ((double)total_ops_found / count_found) : 0;
            double avg_ops_not_found = (count_not_found > 0) ? ((double)total_ops_not_found / count_not_found) : 0;

            total_avg_ops_found += avg_ops_found;
            total_max_ops_found += max_ops_found;
            total_avg_ops_not_found += avg_ops_not_found;
            total_max_ops_not_found += max_ops_not_found;

            delete[] hashTable;
            delete[] search_keys;
        }

        data[i][0] = (double)alpha;
        data[i][1] = total_avg_ops_found / NR_TESTS;
        data[i][2] = (double)total_max_ops_found / NR_TESTS;
        data[i][3] = total_avg_ops_not_found / NR_TESTS;
        data[i][4] = (double)total_max_ops_not_found / NR_TESTS;
    }

    addGroup(filename, "Before Deletion", data, num_alphas);
    delete[] a;
}

void profileAfterDeletion(const char* filename) {
    int* a = new int[N];
    
    float alpha_initial = 0.99;
    float alpha_final = 0.80;

    double total_avg_ops_found = 0;
    double total_avg_ops_not_found = 0;

    int total_max_ops_found = 0;
    int total_max_ops_not_found = 0;

    for (int test = 0; test < NR_TESTS; test++) {
        int n_initial = (int)(alpha_initial * N);
        int n_final = (int)(alpha_final * N);
        int n_to_delete = n_initial - n_final;

        Entry* hashTable = new Entry[N];
        for (int j = 0; j < N; j++) {
            hashTable[j].id = EMPTY_ID;
            hashTable[j].name[0] = '\0';
        }

        FillRandomArray(a, n_initial, MIN_INSERT_VAL, MAX_INSERT_VAL, true, UNSORTED);
        for (int j = 0; j < n_initial; j++) {
            Entry entry;
            entry.id = a[j];
            hash_insert(hashTable, N, entry, false);
        }

        shuffle(a, n_initial);
        for (int j = 0; j < n_to_delete; j++) {
            hash_delete(hashTable, N, a[j], false);
        }

        const int m = 3000;
        const int m_found = 1500;
        const int m_not_found = m - m_found;
        int* search_keys = new int[m];
        
        for (int j = 0; j < m_found; j++) {
            search_keys[j] = a[n_to_delete + (rand() % (n_initial - n_to_delete))];
        }

        int m_deleted = m_not_found / 2;
        
        for (int j = m_found; j < m_found + m_deleted; j++) {
            search_keys[j] = a[rand() % n_to_delete];
        }

        for (int j = m_found + m_deleted; j < m; j++) {
            search_keys[j] = MIN_NOTFOUND_VAL + (rand() % (MAX_NOTFOUND_VAL - MIN_NOTFOUND_VAL + 1));
        }

        shuffle(search_keys, m);

        long long total_ops_found = 0;
        long long total_ops_not_found = 0;

        int count_found = 0;
        int count_not_found = 0;

        int max_ops_found = 0;
        int max_ops_not_found = 0;

        for (int j = 0; j < m; j++) {
            int ops = 0;
            Entry* result = hash_search(hashTable, N, search_keys[j], ops);

            if (result != nullptr) {
                total_ops_found += ops;
                count_found++;
                if (ops > max_ops_found) max_ops_found = ops;
            } else {
                total_ops_not_found += ops;
                count_not_found++;
                if (ops > max_ops_not_found) max_ops_not_found = ops;
            }
        }

        double avg_ops_found = (count_found > 0) ? ((double)total_ops_found / count_found) : 0;
        double avg_ops_not_found = (count_not_found > 0) ? ((double)total_ops_not_found / count_not_found) : 0;

        total_avg_ops_found += avg_ops_found;
        total_max_ops_found += max_ops_found;
        total_avg_ops_not_found += avg_ops_not_found;
        total_max_ops_not_found += max_ops_not_found;

        delete[] hashTable;
        delete[] search_keys;
    }

    double data[1][5];
    data[0][0] = (double)alpha_final;
    data[0][1] = total_avg_ops_found / NR_TESTS;
    data[0][2] = (double)total_max_ops_found / NR_TESTS;
    data[0][3] = total_avg_ops_not_found / NR_TESTS;
    data[0][4] = (double)total_max_ops_not_found / NR_TESTS;

    char group_name[100];
    sprintf(group_name, "After Deletion (%.2f -> %.2f)", alpha_initial, alpha_final);
    addGroup(filename, group_name, data, 1);

    delete[] a;
}

void perf_all() {
    const char* filename = "hash_table_report.html";
    initReport(filename, "Effort measurements at various filling factors");
    perfBeforeDeletion(filename);
    profileAfterDeletion(filename);
    finaliseReport(filename);
}

int main() {
    srand(time(nullptr));
    perf_all();
    return 0;
}