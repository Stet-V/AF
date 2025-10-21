#include <stdio.h>
#include <stdlib.h>
#include "Profiler.h"

#define MAX_SIZE 10000
#define STEP_SIZE 500
#define MAX_THRESHOLD 50
#define NR_TESTS 10
#define OPTIMAL_THRESHOLD 15

int m = 5;

enum algorithm {
    QUICKSORT = 0,
    HEAPSORT = 1,
    HYBRID_QUICKSORT = 2,
    QUICKSELECT = 3
};

enum testMode {
    QUICKSORT_ANALYSIS = 0,
    HEAPSORT_VS_QUICKSORT = 1,
    HYBRID_THRESHOLD_ANALYSIS = 2,
    QUICKSORT_VS_HYBRID_QUICKSORT = 3
};

Profiler p;

void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

void populateArray(int src[], int dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

void maxHeapify(int a[], int n, int i, Operation compare, Operation assign) {
    
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

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

void heapSort(int a[], int n) {

    Operation heapSortCompare = p.createOperation("heapsort-comparisons", n);
    Operation heapSortAssign = p.createOperation("heapsort-assignments", n);

    for (int i = n / 2 - 1; i >= 0; i--) {
        maxHeapify(a, n, i, heapSortCompare, heapSortAssign);
    }

    int heap_size = n;
    for (int i = n - 1; i > 0; i--) {
        heapSortAssign.count(3);
        swap(a[0], a[i]);
        heap_size--;
        maxHeapify(a, heap_size, 0, heapSortCompare, heapSortAssign);
    }
}

int partition(int a[], int left, int right, Operation compare, Operation assign) {

    int pivot = a[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        compare.count();
        if (a[j] <= pivot) {
            i++;
            assign.count(3);
            swap(a[i], a[j]);
        }
    }
    assign.count(3);
    swap(a[i + 1], a[right]);
    return i + 1;
}

void quickSort(int a[], int left, int right, Operation compare, Operation assign) {
    if (left < right) {
        int pivotIndex = partition(a, left, right, compare, assign);
        quickSort(a, left, pivotIndex - 1, compare, assign);
        quickSort(a, pivotIndex + 1, right, compare, assign);
    }
}

void quickSortWrapper(int a[], int n) {
    Operation quickSortCompare = p.createOperation("quicksort-comparisons", n);
    Operation quickSortAssign = p.createOperation("quicksort-assignments", n);

    quickSort(a, 0, n - 1, quickSortCompare, quickSortAssign);
}

void generateBestCaseQuickSort(int a[], int left, int right) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    generateBestCaseQuickSort(a, left, mid - 1);
    generateBestCaseQuickSort(a, mid + 1, right);
    swap(a[right], a[mid]);
}

void insertionSort(int a[], int n, Operation compare, Operation assign) {
    for (int i = 1; i < n; i++) {   
        assign.count();
        int key = a[i];
        int left = 0, right = i - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            compare.count();
            if (a[mid] > key) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        for (int j = i - 1; j >= left; j--) {
            assign.count();
            a[j + 1] = a[j];
        }
        assign.count();
        a[left] = key;
    }
}

void hybridQuickSort(int a[], int left, int right, int threshold, Operation compare, Operation assign) {
    if (right - left + 1 <= threshold) {
        insertionSort(a + left, right - left + 1, compare, assign);
    } else {
        if (left < right) {
            int pivotIndex = partition(a, left, right, compare, assign);
            hybridQuickSort(a, left, pivotIndex - 1, threshold, compare, assign);
            hybridQuickSort(a, pivotIndex + 1, right, threshold, compare, assign);
        }
    }
}

void hybridQuickSortWrapper(int a[], int n, int threshold) {
    Operation hybridQuickSortCompare = p.createOperation("hybrid-quicksort-comparisons", n);
    Operation hybridQuickSortAssign = p.createOperation("hybrid-quicksort-assignments", n);

    hybridQuickSort(a, 0, n - 1, threshold, hybridQuickSortCompare, hybridQuickSortAssign);
}

int randomisedPartition(int a[], int left, int right) {

    int randomPivot = left + rand() % (right - left + 1);
    swap(a[randomPivot], a[right]);

    int pivot = a[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (a[j] <= pivot) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i + 1], a[right]);
    return i + 1;
}

void quickSelect(int a[], int left, int right) {
    if (left < right) {
        int pivotIndex = randomisedPartition(a, left, right);
        quickSelect(a, left, pivotIndex - 1);
        quickSelect(a, pivotIndex + 1, right);
    }
}

void demo(int algorithm) {
    int a[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(a)/sizeof(a[0]);

    switch (algorithm) {
        case QUICKSORT: {
            int quickSortArray[n];

            populateArray(a, quickSortArray, n);
            quickSortWrapper(quickSortArray, n);

            printf("Quicksort: \n");
            for (int i = 0; i < n; i++) {
                printf("%d ", quickSortArray[i]);
            }
            printf("\n");
            break;
        }
        case HEAPSORT: {
            int heapSortArray[n];

            populateArray(a, heapSortArray, n);
            heapSort(heapSortArray, n);

            printf("Heapsort: \n");
            for (int i = 0; i < n; i++) {
                printf("%d ", heapSortArray[i]);
            }
            printf("\n");
            break;
        }
        case HYBRID_QUICKSORT: {
            int hybridQuickSortArray[n];

            populateArray(a, hybridQuickSortArray, n);
            hybridQuickSortWrapper(hybridQuickSortArray, n, OPTIMAL_THRESHOLD);

            printf("Hybrid Quicksort: \n");
            for (int i = 0; i < n; i++) {
                printf("%d ", hybridQuickSortArray[i]);
            }
            printf("\n");
        
            break;
        }
        case QUICKSELECT: {
            int quickSelectArray[n];

            populateArray(a, quickSelectArray, n);
            quickSelect(quickSelectArray, 0, n - 1);

            printf("QuickSelect: \n");
            for (int i = 0; i < n; i++) {
                printf("%d ", quickSelectArray[i]);
            }
            printf("\n");
        
            break;
        }
        default:
            break;
    }
}

void demo_all() {
    demo(QUICKSORT);
    demo(HEAPSORT);
    demo(HYBRID_QUICKSORT);
    demo(QUICKSELECT);
}

void perf(int algorithm, int order) {
    int* a = new int[MAX_SIZE];
    int n;
    for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE) {
        for (int i = 0; i < m; i++) {
            FillRandomArray(a, n, 10, 50000, false, order);
            switch (algorithm) {
                case QUICKSORT: {
                    int* quickSortArray = new int[n];

                    populateArray(a, quickSortArray, n);
                    if (order == ASCENDING) {
                        generateBestCaseQuickSort(quickSortArray, 0, n - 1);
                    }
                    quickSortWrapper(quickSortArray, n);

                    if (order == UNSORTED) {
                        int* quickSortArrayTime = new int[n];
                        p.startTimer("quicksort-time", n);
                        for (int j = 0; j < NR_TESTS; j++) {
                            populateArray(a, quickSortArrayTime, n);
                            quickSortWrapper(quickSortArrayTime, n);
                        }
                        p.stopTimer("quicksort-time", n);
                        delete[] quickSortArrayTime;
                    }  

                    delete[] quickSortArray;
                    break;
                }
                case HEAPSORT: {
                    int* heapSortArray = new int[n];

                    populateArray(a, heapSortArray, n);
                    heapSort(heapSortArray, n);

                    delete[] heapSortArray;
                    break;
                }
                case HYBRID_QUICKSORT: {
                    int* hybridQuickSortArray = new int[n];

                    populateArray(a, hybridQuickSortArray, n);
                    hybridQuickSortWrapper(hybridQuickSortArray, n, OPTIMAL_THRESHOLD);

                    p.startTimer("hybrid-quicksort-time", n);
                    for (int j = 0; j < NR_TESTS; j++) {
                        populateArray(a, hybridQuickSortArray, n);
                        hybridQuickSortWrapper(hybridQuickSortArray, n, OPTIMAL_THRESHOLD);
                    }
                    p.stopTimer("hybrid-quicksort-time", n);

                    delete[] hybridQuickSortArray;
                    break;
                }
                default:
                    break;
            }
        }
    }
    delete[] a;
    switch (algorithm) {
        case QUICKSORT: {
            p.divideValues("quicksort-comparisons", m);
            p.divideValues("quicksort-assignments", m);
            p.addSeries("quicksort-total", "quicksort-comparisons", "quicksort-assignments");

            p.createGroup("Quicksort Operations", "quicksort-comparisons", "quicksort-assignments", "quicksort-total");
            break;
        }
        case HEAPSORT: {
            p.divideValues("heapsort-comparisons", m);
            p.divideValues("heapsort-assignments", m);
            p.addSeries("heapsort-total", "heapsort-comparisons", "heapsort-assignments");

            p.createGroup("Heapsort Operations", "heapsort-comparisons", "heapsort-assignments", "heapsort-total");
            break;
        }
        case HYBRID_QUICKSORT: {
            p.divideValues("hybrid-quicksort-comparisons", m);
            p.divideValues("hybrid-quicksort-assignments", m);
            p.addSeries("hybrid-quicksort-total", "hybrid-quicksort-comparisons", "hybrid-quicksort-assignments");

            p.createGroup("Hybrid Quicksort Operations", "hybrid-quicksort-comparisons", "hybrid-quicksort-assignments", "hybrid-quicksort-total");
            break;
        }
        default:
            break;
    }
}

void analyzeHybridThreshold() {
    int* hybridQuickSortArray = new int[MAX_SIZE];

    for (int threshold = 5; threshold <= MAX_THRESHOLD; threshold++) {
        Operation hybridQuickSortThresholdCompare = p.createOperation("hybrid-quicksort-threshold-comparisons", threshold);
        Operation hybridQuickSortThresholdAssign = p.createOperation("hybrid-quicksort-threshold-assignments", threshold);
        for (int i = 0; i < NR_TESTS; i++) {
            FillRandomArray(hybridQuickSortArray, MAX_SIZE, 10, 50000, false, UNSORTED);
            hybridQuickSort(hybridQuickSortArray, 0, MAX_SIZE - 1, threshold, hybridQuickSortThresholdCompare, hybridQuickSortThresholdAssign);
        }
    }
    delete[] hybridQuickSortArray;
    p.divideValues("hybrid-quicksort-threshold-comparisons", NR_TESTS);
    p.divideValues("hybrid-quicksort-threshold-assignments", NR_TESTS);
    
    p.addSeries("hybrid-quicksort-threshold-total", "hybrid-quicksort-threshold-comparisons", "hybrid-quicksort-threshold-assignments");
    p.createGroup("Hybrid Quicksort Threshold Analysis", "hybrid-quicksort-threshold-comparisons", "hybrid-quicksort-threshold-assignments", "hybrid-quicksort-threshold-total");
}

void perfAnalysis(int testMode) {
    switch (testMode) {
        case QUICKSORT_ANALYSIS: {
            p.reset("Quicksort Average Case Analysis");
            perf(QUICKSORT, UNSORTED);

            p.reset("Quicksort Best Case Analysis");
            perf(QUICKSORT, ASCENDING);

            p.reset("Quicksort Worst Case Analysis");
            perf(QUICKSORT, DESCENDING);
            break;
        }
        case HEAPSORT_VS_QUICKSORT: {
            p.reset("Heapsort vs Quicksort - Average Case");
            perf(HEAPSORT, UNSORTED);
            perf(QUICKSORT, UNSORTED);

            p.createGroup("Heapsort vs Quicksort", "heapsort-total", "quicksort-total");
            break;
        }
        case HYBRID_THRESHOLD_ANALYSIS: {
            p.reset("Hybrid Quicksort Threshold Analysis");
            analyzeHybridThreshold();
            break;
        }
        case QUICKSORT_VS_HYBRID_QUICKSORT: {
            p.reset("Quicksort vs Hybrid Quicksort - Average Case");
            perf(QUICKSORT, UNSORTED);
            perf(HYBRID_QUICKSORT, UNSORTED);

            p.createGroup("Quicksort vs Hybrid Quicksort", "quicksort-total", "hybrid-quicksort-total");
            p.createGroup("Quicksort vs Hybrid Quicksort Times", "quicksort-time", "hybrid-quicksort-time");
            break;
        }
        default:
            break;
    }
}

void perf_all() {
    perfAnalysis(QUICKSORT_ANALYSIS);

    perfAnalysis(HEAPSORT_VS_QUICKSORT);

    perfAnalysis(HYBRID_THRESHOLD_ANALYSIS);

    perfAnalysis(QUICKSORT_VS_HYBRID_QUICKSORT);
    p.showReport();
}

int main() {
    // printf("Hello World!\n");
    // demo(HEAPSORT);
    // demo(QUICKSORT);
    // perfAnalysis(QUICKSORT_ANALYSIS);
    // perf(HEAPSORT, UNSORTED);
    // perfAnalysis(HEAPSORT_VS_QUICKSORT);
    // demo(HYBRID_QUICKSORT);
    // perfAnalysis(HYBRID_THRESHOLD_ANALYSIS);
    // perfAnalysis(QUICKSORT_VS_HYBRID_QUICKSORT);
    // demo_all();
    perf_all();
    // p.showReport();
    return 0;
}