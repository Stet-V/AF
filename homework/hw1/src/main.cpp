/*
    Stet Vasile Alexandru, Grupa 10
    am implementat algoritmii de sortare bubble sort, selection sort, insertion sort, si insertion sort folosind insertie binara (v2)
    am observat ca pe average case, cel mai buni algoritm este insertion sort (v2), urmat de selection sort, care este marginal mai eficient
    decat insertion sort (sunt aproximativ egale), si in final bubble sort
    pe best case, bubble sort este cel mai bun, apoi insertion sort, apoi insertion sort (v2), si in final selection sort
    pe worst case, selection sort este cel mai bun, apoi insertion sort (v2), apoi insertion sort, si in final bubble sort
    cu toate acestea, toti algoritmii au complexitatea O(n^2) in average si worst case
    pe best case, bubble sort are complexitatea O(n), insertion sort are complexitatea O(n), insertion sort (v2) are complexitatea O(n log n),
    iar selection sort ramane O(n^2)
*/

#include <stdio.h>
#include <stdlib.h>
#include "Profiler.h"

#define MAX_SIZE 10000
#define STEP_SIZE 100

int m = 5;

Profiler p("sortingAlgorithms");

void bubbleSort(int a[], int n) {

    Operation opCompare = p.createOperation("bubble-comparisons", n);
    Operation opAssign = p.createOperation("bubble-assignments", n);

    bool swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            opCompare.count();
            if (a[j] > a[j+1]) {
                opAssign.count(3);
                int temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
                swapped = true;
            }
        }
        if (swapped == false) {
            break;
        }
    }
}

void selectionSort(int a[], int n) {

    Operation opCompare = p.createOperation("selection-comparisons", n);
    Operation opAssign = p.createOperation("selection-assignments", n);

    for (int i = 0; i < n - 1; i++) {
        int index = i;
        for (int j = i+1; j < n; j++) {
            opCompare.count();
            if (a[j] < a[index]) {
                index = j;
            }
        }
        if (index != i) {
            opAssign.count(3);
            int temp = a[index];
            a[index] = a[i];
            a[i] = temp;
        }
    }
}

void insertionSort(int a[], int n) {

    Operation opCompare = p.createOperation("insertion-comparisons", n);
    Operation opAssign = p.createOperation("insertion-assignments", n);

    for (int i = 1; i < n; i++) {
        opAssign.count();
        int key = a[i];
        int j = i - 1;
        while (j >= 0) {
            opCompare.count();
            if (a[j] > key) {
                opAssign.count();
                a[j + 1] = a[j];
                j--;
            } else {
                break; 
            }
        }
        opAssign.count();
        a[j + 1] = key;
    }
}

void insertionSort_v2(int a[], int n) {

    Operation opCompare = p.createOperation("insertion_v2-comparisons", n);
    Operation opAssign = p.createOperation("insertion_v2-assignments", n);

    for (int i = 1; i < n; i++) {
        opAssign.count();
        int key = a[i];
        int left = 0, right = i - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            opCompare.count();
            if (a[mid] > key) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        for (int j = i - 1; j >= left; j--) {
            opAssign.count();
            a[j + 1] = a[j];
        }
        opAssign.count();
        a[left] = key;
    }
}

void demo() {
    int a[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(a)/sizeof(a[0]);
    int bubbleArr[n], selectionArr[n], insertionArr[n], insertionArr_v2[n];
    for (int i=0; i < n; i++) {
        bubbleArr[i] = a[i];
        selectionArr[i] = a[i];
        insertionArr[i] = a[i];
        insertionArr_v2[i] = a[i];
    }
    bubbleSort(bubbleArr, n);
    for (int i=0; i < n; i++)
        printf("%d ", bubbleArr[i]);
    printf("\n");
    selectionSort(selectionArr, n);
    for (int i=0; i < n; i++)
        printf("%d ", selectionArr[i]);
    printf("\n");
    insertionSort(insertionArr, n);
    for (int i=0; i < n; i++)
        printf("%d ", insertionArr[i]);
    printf("\n");
    insertionSort_v2(insertionArr_v2, n);
    for (int i=0; i < n; i++)
        printf("%d ", insertionArr_v2[i]);
    printf("\n");
}

void perf(int order) {
    int* a = new int[MAX_SIZE];
    int n;
    for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE) {
        for (int i = 0; i < m; i++) {
            FillRandomArray(a, n, 10, 50000, false, order);

            int* bubbleArr = new int[n];
            int* selectionArr = new int[n];
            int* insertionArr = new int[n];
            int* insertionArr_v2 = new int[n];

            for (int j = 0; j < n; j++) {
                bubbleArr[j] = a[j];
                selectionArr[j] = a[j];
                insertionArr[j] = a[j];
                insertionArr_v2[j] = a[j];
            }

            if (order == DESCENDING) {
                int temp = insertionArr[0];
                insertionArr[0] = insertionArr[n-1];
                insertionArr[n-1] = temp;

                temp = insertionArr_v2[0];
                insertionArr_v2[0] = insertionArr_v2[n-1];
                insertionArr_v2[n-1] = temp;
            }

            bubbleSort(bubbleArr, n);
            selectionSort(selectionArr, n);
            insertionSort(insertionArr, n);
            insertionSort_v2(insertionArr_v2, n);

            delete[] bubbleArr;
            delete[] selectionArr;
            delete[] insertionArr;
            delete[] insertionArr_v2;
        }
    }

    p.divideValues("bubble-comparisons", m);
    p.divideValues("bubble-assignments", m);
    p.addSeries("bubble-total", "bubble-comparisons", "bubble-assignments");

    p.divideValues("selection-comparisons", m);
    p.divideValues("selection-assignments", m);
    p.addSeries("selection-total", "selection-comparisons", "selection-assignments");

    p.divideValues("insertion-comparisons", m);
    p.divideValues("insertion-assignments", m);
    p.addSeries("insertion-total", "insertion-comparisons", "insertion-assignments");

    p.divideValues("insertion_v2-comparisons", m);
    p.divideValues("insertion_v2-assignments", m);
    p.addSeries("insertion_v2-total", "insertion_v2-comparisons", "insertion_v2-assignments");

    if (order == UNSORTED) {
        p.createGroup("Linear and log-linear curves", "insertion_v2-comparisons", "selection-assignments");
    }

    if (order == ASCENDING) {
        p.createGroup("Linear and log-linear curves", "bubble-total", "insertion_v2-total", "insertion-total");
    }

    if (order == DESCENDING) {
        p.createGroup("Linear and log-linear curves", "insertion_v2-comparisons", "selection-assignments");
    }

    p.createGroup("Bubble Sort", "bubble-comparisons", "bubble-assignments", "bubble-total");
    p.createGroup("Selection Sort", "selection-comparisons", "selection-assignments", "selection-total");
    p.createGroup("Insertion Sort", "insertion-comparisons", "insertion-assignments", "insertion-total");
    p.createGroup("Insertion_v2 Sort", "insertion_v2-comparisons", "insertion_v2-assignments", "insertion_v2-total");

    p.createGroup("All Sorts", "bubble-total", "selection-total", "insertion-total", "insertion_v2-total");
}

void perf_all() {
    perf(UNSORTED);
    p.reset("sortingAlgorithms - best case");
    perf(ASCENDING);
    p.reset("sortingAlgorithms - worst case");
    perf(DESCENDING);
    p.showReport();
}

int main() {
    // printf("Hello, World!\n");
    // demo();
    // perf();
    perf_all();
    return 0;
}