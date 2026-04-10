#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

MPI_Status status;

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: mpirun -np <NP> ./matmul <N> \"Nama - NPM\"\n");
        return 1;
    }

    int numtasks, taskid, numworkers, source, dest, i, j, k;
    int N = atoi(argv[1]);
    char *info = argv[2];
    double comp_time = 0.0, comm_time = 0.0;
    double t_start, t_end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    numworkers = numtasks - 1;

    double *a = (double *)malloc(N * N * sizeof(double));
    double *b = (double *)malloc(N * N * sizeof(double));
    double *c = (double *)malloc(N * N * sizeof(double));

    if (!a || !b || !c) {
        if (taskid == 0) fprintf(stderr, "Memory allocation failed for N=%d\n", N);
        MPI_Finalize();
        return 1;
    }

    /*---------------------------- master ----------------------------*/
    if (taskid == 0) {
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++) {
                a[i * N + j] = 1.0;
                b[i * N + j] = 2.0;
            }

        int base_rows = N / numworkers;
        int remainder  = N % numworkers;
        int offset = 0;

        /* send data to workers */
        t_start = MPI_Wtime();
        for (dest = 1; dest <= numworkers; dest++) {
            int rows = base_rows + (dest <= remainder ? 1 : 0);
            MPI_Send(&offset, 1,          MPI_INT,    dest, 1, MPI_COMM_WORLD);
            MPI_Send(&rows,   1,          MPI_INT,    dest, 1, MPI_COMM_WORLD);
            MPI_Send(a + offset * N, rows * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            MPI_Send(b, N * N,        MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            offset += rows;
        }

        /* receive results from workers */
        for (i = 1; i <= numworkers; i++) {
            int off, rows;
            MPI_Recv(&off,  1, MPI_INT,    i, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT,    i, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(c + off * N, rows * N, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
        }
        t_end = MPI_Wtime();
        comm_time = t_end - t_start;

        /* gather comp time from workers (take max) */
        double worker_comp;
        for (i = 1; i <= numworkers; i++) {
            MPI_Recv(&worker_comp, 1, MPI_DOUBLE, i, 3, MPI_COMM_WORLD, &status);
            if (worker_comp > comp_time) comp_time = worker_comp;
        }

        printf("Info      : %s\n", info);
        printf("Matrix N  : %d x %d\n", N, N);
        printf("Processors: %d\n", numtasks);
        printf("Comp Time : %.4f s\n", comp_time);
        printf("Comm Time : %.4f s\n", comm_time);
    }

    /*---------------------------- worker ----------------------------*/
    if (taskid > 0) {
        int offset, rows;
        MPI_Recv(&offset, 1, MPI_INT,    0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows,   1, MPI_INT,    0, 1, MPI_COMM_WORLD, &status);

        double *local_a = (double *)malloc(rows * N * sizeof(double));
        double *local_c = (double *)malloc(rows * N * sizeof(double));

        MPI_Recv(local_a, rows * N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(b,       N * N,    MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);

        /* matrix multiplication */
        t_start = MPI_Wtime();
        for (i = 0; i < rows; i++)
            for (j = 0; j < N; j++) {
                local_c[i * N + j] = 0.0;
                for (k = 0; k < N; k++)
                    local_c[i * N + j] += local_a[i * N + k] * b[k * N + j];
            }
        t_end = MPI_Wtime();
        comp_time = t_end - t_start;

        MPI_Send(&offset,   1,        MPI_INT,    0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows,     1,        MPI_INT,    0, 2, MPI_COMM_WORLD);
        MPI_Send(local_c, rows * N,   MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&comp_time, 1,       MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);

        free(local_a);
        free(local_c);
    }

    free(a);
    free(b);
    free(c);
    MPI_Finalize();
    return 0;
}
