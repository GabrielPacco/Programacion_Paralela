#include <omp.h>
#include <stdio.h>

void oddEvenTranspositionSort(int *a, int n, int thread_count) {
    int phase, i, tmp;

    for (phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            #pragma omp parallel for num_threads(thread_count) default(none) shared(a, n) private(i, tmp)
            for (i = 1; i < n; i += 2) {
                if (a[i-1] > a[i]) {
                    tmp = a[i-1];
                    a[i-1] = a[i];
                    a[i] = tmp;
                }
            }
        } else {
            #pragma omp parallel for num_threads(thread_count) default(none) shared(a, n) private(i, tmp)
            for (i = 1; i < n-1; i += 2) {
                if (a[i] > a[i+1]) {
                    tmp = a[i+1];
                    a[i+1] = a[i];
                    a[i] = tmp;
                }
            }
        }
    }
}

int main() {
    int n = 4; // Tamaño del arreglo
    int a[] = {9, 7, 8, 6}; // Arreglo a ordenar
    int thread_count = 4; // Número de hilos

    oddEvenTranspositionSort(a, n, thread_count);

    printf("Arreglo ordenado:\n");
    for(int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
