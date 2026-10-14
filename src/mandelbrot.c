#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Función para calcular el valor de Mandelbrot
int mandel_val(double x, double y, int maxiter) {
    double zr = 0.0, zi = 0.0;
    int iter = 0;
    while (zr * zr + zi * zi <= 4.0 && iter < maxiter) {
        double temp = zr * zr - zi * zi + x;
        zi = 2.0 * zr * zi + y;
        zr = temp;
        iter++;
    }
    return iter;
}

int main() {
    int m = 1000; // Dimensión x / filas (i)
    int n = 1000; // Dimensión y / columnas (j)
    int maxiter = 200;

    // Reserva dinámica de memoria para las matrices depth[j][i] y dith_depth[j][i]
    int **depth = (int **)malloc(n * sizeof(int *));
    double **dith_depth = (double **)malloc(n * sizeof(double *));
    for (int j = 0; j < n; j++) {
        depth[j] = (int *)malloc(m * sizeof(int));
        dith_depth[j] = (double *)malloc(m * sizeof(double));
    }

    // =========================================================================
    // INSTRUMENTACIÓN: Captura del tiempo inicial de ejecución
    // =========================================================================
    double t_start = omp_get_wtime();

    // Región paralela (Modelo SPMD del Ejemplo 2.10)
    #pragma omp parallel
    {
        // Al declarar las variables dentro del bloque paralelo en C,
        // automáticamente son privadas para cada hilo.
        int nthreads = omp_get_num_threads();
        int my_thread = omp_get_thread_num();

        // Generalización del cálculo de rango para cualquier número de hilos
        int my_width = m / nthreads;
        int i_start = my_thread * my_width;
        int i_end = (my_thread == nthreads - 1) ? m : (i_start + my_width);

        // =====================================================================
        // INSTRUMENTACIÓN: Imprime qué hilo procesa qué rango de filas
        // =====================================================================
        #pragma omp critical
        {
            printf("[Hilo %d/%d] Asignado rango de filas (i): %d a %d\n",
                   my_thread, nthreads, i_start, i_end - 1);
        }

        // 1. Primer bucle: Generación del conjunto de Mandelbrot
        for (int i = i_start; i < i_end; i++) {
            for (int j = 0; j < n; j++) {
                double x = (double)i / (double)m;
                double y = (double)j / (double)n;
                depth[j][i] = mandel_val(x, y, maxiter);
            }
        }

        // 2. Segundo bucle: Difuminado (Dithering)
        for (int i = i_start; i < i_end; i++) {
            for (int j = 0; j < n; j++) {
                // Manejo de bordes para evitar accesos fuera de rango (j-1 y j+1)
                int j_left = (j > 0) ? (j - 1) : j;
                int j_right = (j < n - 1) ? (j + 1) : j;

                dith_depth[j][i] = 0.5 * depth[j][i] +
                                   0.25 * (depth[j_left][i] + depth[j_right][i]);
            }
        }
    } // Fin de la región paralela (#pragma omp parallel)

    // =========================================================================
    // INSTRUMENTACIÓN: Captura del tiempo final y cálculo del tiempo total
    // =========================================================================
    double t_end = omp_get_wtime();
    double t_total = t_end - t_start;

    printf("\n--> Tiempo total de ejecucion: %.6f segundos\n", t_total);

    // Liberación de memoria dinámica
    for (int j = 0; j < n; j++) {
        free(depth[j]);
        free(dith_depth[j]);
    }
    free(depth);
    free(dith_depth);

    return 0;
}