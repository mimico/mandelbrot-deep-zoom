#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "omp.h"
#include "julia.h"

void julia (int         width,
            int         height,
            double      *x,
            double      *y,
            int         maxiter,
            int         my_rank,
            float      *iterations,
            int         processes)
{
  int row;

  x[1] = x[1] - x[0];
  double xgap = x[1] / width;

  y[1] = y[1] - y[0];
  double ygap = y[1] / height;

/* On Blue Gene/Q, the pragma directive forked the image into 4
 * processes, dynamically, since the work performed on each part
 * could vary widely. On mpihost and Velocity (a 4 core hyperthreaded
 * desktop - 8 logical cores, used for some of the images), this directive
 * was commented out. */
# pragma omp parallel for num_threads (thread_count) schedule (dynamic)
  for (row = 0; row < height; row++) {
    float *iterations_row = iterations + (width * row);

    /* Calculate the value for the pixels in each row. */
    julia_row (width, xgap, ygap, x[0], y[0], row, maxiter, iterations_row);
  }
}
