#include <omp.h>
#include <stdio.h>

int main(void) {
    int threads = 0;

    #pragma omp parallel
    {
        #pragma omp single
        threads = omp_get_num_threads();

        printf("Hola desde el hilo %d de %d\n",
               omp_get_thread_num(),
               omp_get_num_threads());
    }

    printf("Total de hilos: %d\n", threads);

    return 0;
}