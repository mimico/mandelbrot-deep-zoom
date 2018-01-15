#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "gmp.h"
#include "mpi.h"
#include "julia.h"

void julia (int           width,
            int           height,
            mpf_t        *x,
            mpf_t        *y,
            int           maxiter,
            int           my_rank,
            float          *iterations,
            int           processes)
{
  int row;

  MPI_Status status;

  mpf_sub (x[1], x[1], x[0]);
  double t1 = mpf_get_d (x[1]);
  double xgap = (t1) / width; /* spacing between points in x */

  mpf_sub (y[1], y[1], y[0]);
  double t2 = mpf_get_d (y[1]);
  double ygap = (t2) / height; /* spacing between points in y */

  /* Save all iteration_rows in my_iterations,
   * so we can send them all at the end. */
  int max_rows = (height + processes - 1) / processes;
  float *my_iterations = malloc (sizeof (float) * width * max_rows);
  int n_rows_done = 0;

  /* Each process processes rows of the image according to their rank. */
  for (row = my_rank; row < height; row += processes) {
    float *iterations_row;

    if (my_rank != 0)
      iterations_row = my_iterations + (width * n_rows_done++);
    else
      iterations_row = iterations + (width * row);

    julia_row (width, xgap, ygap, x[0], y[0], row, maxiter, iterations_row);
  }

  /* Collect the rows. */
  if (my_rank != 0) {
    /* Workers have to send their rows to the master. */
    for (row = 0; row < n_rows_done; row++) {
      MPI_Send (my_iterations + (width * row), width, MPI_FLOAT, JULIA_MASTER, DATA, MPI_COMM_WORLD);
    }
  } else {
    /* Master has to receive all of the rows */
    int i;

    for (i = 0; i < height; i++) {
      float *iterations_row = (iterations + i*width);
      int worker = (i % processes);

      /* receive work back from the workers */
      if (worker)
        MPI_Recv (iterations_row, width, MPI_FLOAT, worker, DATA, MPI_COMM_WORLD, &status);
    }
  }

  free (my_iterations);
}
