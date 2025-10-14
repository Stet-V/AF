/*
    Stet Vasile Alexandru, Grupa 10
    Am implementat algoritmul de sortare Bubble Sort in varianta iterativa si recursiva
    Am implementat algoritmul de sortare Heap Sort folosind doua metode de construire a heap-ului: bottom-up (heapify, cu complexitate O(n))
    si top-down (inserari succesive cu sift-up, cu complexitate O(n log n)).
    Totusi, am observat ca ambele variante de Heap Sort au performante similare pe date aleatoare, cu o usoara diferenta in favoarea variantei bottom-up.
    Am observat ca Bubble Sort recursiv este usor mai lent decat cel iterativ din cauza overhead-ului apelurilor recursive;
    de asemenea, poate provoca stack overflow pentru dimensiuni mari (adancime recursivă O(n)).
    Complexitati: Bubble Sort - O(n^2) in mediu/rau, O(n) în cel mai bun caz; Heap Sort - O(n log n) in toate cazurile.    
*/

#include <stdio.h>
#include <stdlib.h>
#include "Profiler.h"

#define MAX_SIZE 10000
#define STEP_SIZE 100

int m = 5;
int nr_tests = 5;

enum algorithm {
    BUBBLE = 0,
    HEAPSORT = 1
};

Profiler p("HeapSort - Average Case");

void bubbleSort(int a[], int n) {

    Operation compare = p.createOperation("bubble-comparisons", n);
    Operation assign = p.createOperation("bubble-assignments", n);

    bool swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            compare.count();
            if (a[j] > a[j+1]) {
                assign.count(3);
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

void recursiveBubbleSort(int a[], int n, Operation compare, Operation assign) {
    if (n == 1) {
        return;
    }
    bool swapped = false;
    for (int j = 0; j < n - 1; j++) {
        compare.count();
        if (a[j] > a[j+1]) {
            assign.count(3);
            int temp = a[j];
            a[j] = a[j+1];
            a[j+1] = temp;
            swapped = true;
        }
    }
    if (swapped == false) {
        return;
    }
    recursiveBubbleSort(a, n - 1, compare, assign);
}

void recursiveBubbleSortWrapper(int a[], int n) {
    Operation compare = p.createOperation("recursive-bubble-comparisons", n);
    Operation assign = p.createOperation("recursive-bubble-assignments", n);
    recursiveBubbleSort(a, n, compare, assign);
}

int parent(int i) {
    return (i - 1) / 2;
}

int left(int i) {
    return 2 * i + 1;
}

int right(int i) {
    return 2 * i + 2;
}

void maxHeapify(int a[], int n, int i, Operation compare, Operation assign) {
    
    int largest = i;
    int l = left(i);
    int r = right(i);

    if (l < n) {
        compare.count();
        if (a[l] > a[largest]) {
            largest = l;
        }
    }
    if (r < n) {
        compare.count();
        if (a[r] > a[largest]) {
            largest = r;
        }
    }
    if (largest != i) {
        assign.count(3);
        int temp = a[i];
        a[i] = a[largest];
        a[largest] = temp;
        maxHeapify(a, n, largest, compare, assign);
    }
}

void siftUp(int a[], int n, int i, Operation compare, Operation assign) {
    while (i > 0) {
        compare.count();
        if (a[parent(i)] < a[i]) {
            assign.count(3);
            int temp = a[i];
            a[i] = a[parent(i)];
            a[parent(i)] = temp;
            i = parent(i);
        } else {
            break; 
        }
    }
}

void buildHeapBottomUp(int a[], int n) {

    Operation bottomUpCompare = p.createOperation("heapify-comparisons", n);
    Operation bottomUpAssign = p.createOperation("heapify-assignments", n);

    for (int i = n / 2 - 1; i >= 0; i--) {
        maxHeapify(a, n, i, bottomUpCompare, bottomUpAssign);
    }
}

void buildHeapTopDown(int a[], int n) {

    Operation topDownCompare = p.createOperation("siftup-comparisons", n);
    Operation topDownAssign = p.createOperation("siftup-assignments", n);

    for (int i = 1; i < n; i++) {
        siftUp(a, n, i, topDownCompare, topDownAssign);
    }
}

void HeapSort(int a[], int n, void (*buildHeap)(int[], int)) {

    Operation heapSortCompare = p.createOperation("heapsort-comparisons", n);
    Operation heapSortAssign = p.createOperation("heapsort-assignments", n);

    buildHeap(a, n);
    int heap_size = n;
    for (int i = n - 1; i > 0; i--) {
        heapSortAssign.count(3);
        int temp = a[0];
        a[0] = a[i];
        a[i] = temp;
        heap_size--;
        maxHeapify(a, heap_size, 0, heapSortCompare, heapSortAssign);
    }
}

void demo(int algorithm) {
    int a[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(a)/sizeof(a[0]);

    if (algorithm == BUBBLE) {
        int bubbleArr[n], recursiveBubbleArr[n];
        for (int i = 0; i < n; i++) {
            bubbleArr[i] = a[i];
            recursiveBubbleArr[i] = a[i];
        }

        bubbleSort(bubbleArr, n);
        for (int i = 0; i < n; i++)
            printf("%d ", bubbleArr[i]);
        printf("\n");

        recursiveBubbleSortWrapper(recursiveBubbleArr, n);
        for (int i = 0; i < n; i++)
            printf("%d ", recursiveBubbleArr[i]);
        printf("\n");
    }

    if (algorithm == HEAPSORT) {
        int topDownArr[n], bottomUpArr[n];
        for (int i = 0; i < n; i++) {
            topDownArr[i] = a[i];
            bottomUpArr[i] = a[i];
        }

        buildHeapBottomUp(bottomUpArr, n);
        for (int i = 0; i < n; i++)
            printf("%d ", bottomUpArr[i]);
        printf("\n");

        HeapSort(bottomUpArr, n, buildHeapBottomUp);
        for (int i = 0; i < n; i++)
            printf("%d ", bottomUpArr[i]);
        printf("\n");

        buildHeapTopDown(topDownArr, n);
        for (int i = 0; i < n; i++)
            printf("%d ", topDownArr[i]);
        printf("\n");
        
        HeapSort(topDownArr, n, buildHeapTopDown);
        for (int i = 0; i < n; i++)
            printf("%d ", topDownArr[i]);
        printf("\n");
    }
}

void perf(int algorithm, int order) {
    int* a = new int[MAX_SIZE];
    int n;
    for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE) {
        for (int i = 0; i < m; i++) {
            FillRandomArray(a, n, 10, 50000, false, order);
            if (algorithm == BUBBLE) {
                int* bubbleArr = new int[n];
                int* recursiveBubbleArr = new int[n];

                for (int j = 0; j < n; j++) {
                    bubbleArr[j] = a[j];
                    recursiveBubbleArr[j] = a[j];
                }

                bubbleSort(bubbleArr, n);
                recursiveBubbleSortWrapper(recursiveBubbleArr, n);

                delete[] bubbleArr;
                delete[] recursiveBubbleArr;
            }
            if (algorithm == HEAPSORT) {
                int* bottomUpArr = new int[n];
                int* topDownArr = new int[n];

                for (int j = 0; j < n; j++) {
                    bottomUpArr[j] = a[j];
                    topDownArr[j] = a[j];
                }

                HeapSort(bottomUpArr, n, buildHeapBottomUp);
                HeapSort(topDownArr, n, buildHeapTopDown);

                delete[] bottomUpArr;
                delete[] topDownArr;
            }
        }
        if (algorithm == BUBBLE) {
            int* bubbleArrTime = new int[n];
            int* recursiveBubbleArrTime = new int[n];

            for (int i = 0; i < nr_tests; i++) {
                p.startTimer("bubble-time", n);
                for (int j = 0; j < n; j++) {
                    bubbleArrTime[j] = a[j];
                }
                bubbleSort(bubbleArrTime, n);
                p.stopTimer("bubble-time", n);
            }
        
            for (int i = 0; i < nr_tests; i++) {
                p.startTimer("recursive-bubble-time", n);
                for (int j = 0; j < n; j++) {
                    recursiveBubbleArrTime[j] = a[j];
                }
                recursiveBubbleSortWrapper(recursiveBubbleArrTime, n);
                p.stopTimer("recursive-bubble-time", n);
            }
        }
    }
    delete[] a;
    if (algorithm == BUBBLE) {
        p.divideValues("bubble-comparisons", m);
        p.divideValues("bubble-assignments", m);
        p.addSeries("bubble-total", "bubble-comparisons", "bubble-assignments");

        p.divideValues("recursive-bubble-comparisons", m);
        p.divideValues("recursive-bubble-assignments", m);
        p.addSeries("recursive-bubble-total", "recursive-bubble-comparisons", "recursive-bubble-assignments");

        p.createGroup("Iterative Bubble Operations", "bubble-comparisons", "bubble-assignments", "bubble-total");
        p.createGroup("Recursive Bubble Operations", "recursive-bubble-comparisons", "recursive-bubble-assignments", "recursive-bubble-total");
        p.createGroup("Total Operations", "bubble-total", "recursive-bubble-total");
        p.createGroup("Bubble Times", "bubble-time", "recursive-bubble-time");
    }
    if (algorithm == HEAPSORT) {
        p.divideValues("heapify-comparisons", m);
        p.divideValues("heapify-assignments", m);
        p.addSeries("heapify-total", "heapify-comparisons", "heapify-assignments");

        p.divideValues("siftup-comparisons", m);
        p.divideValues("siftup-assignments", m);
        p.addSeries("siftup-total", "siftup-comparisons", "siftup-assignments");

        p.divideValues("heapsort-comparisons", 2 * m);
        p.divideValues("heapsort-assignments", 2 * m);
        p.addSeries("heapsort-total", "heapsort-comparisons", "heapsort-assignments");

        p.addSeries("heapsort-bottomup-total", "heapify-total", "heapsort-total");
        p.addSeries("heapsort-topdown-total", "siftup-total", "heapsort-total");

        p.createGroup("Heapify (Bottom-Up) Operations", "heapify-comparisons", "heapify-assignments", "heapify-total");
        p.createGroup("Sift-Up (Top-Down) Operations", "siftup-comparisons", "siftup-assignments", "siftup-total");
        p.createGroup("HeapSort (Extraction) Operations", "heapsort-comparisons", "heapsort-assignments", "heapsort-total");
        p.createGroup("Heap Building Operations", "heapify-total", "siftup-total");
        p.createGroup("Total HeapSort Operations", "heapsort-total", "heapsort-bottomup-total", "heapsort-topdown-total");
    }
}

void perf_all() {
    perf(HEAPSORT, UNSORTED);
    p.reset("Heapsort - Worst Case");
    perf(HEAPSORT, ASCENDING);
    p.reset("Bubble - Average Case");
    perf(BUBBLE, UNSORTED);
    p.showReport();
}

int main() {
    // printf("Hello, World!\n");
    // demo(BUBBLE);
    // demo(HEAPSORT);
    // perf(BUBBLE, UNSORTED);
    // perf(HEAPSORT, UNSORTED);
    // perf(HEAPSORT, ASCENDING);
    perf_all();
    return 0;
}