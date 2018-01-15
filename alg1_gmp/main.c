#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "mpi.h"
#include "julia.h"

int main (int argc, char **argv)
{
  int width;
  int height;
  int maxiter;
  int my_rank;
  int processes;
  int start_image;
  int end_image;

  char *image = (char *)(malloc (sizeof (char) * 20));

  /* The following are read in and set from params.dat using getparams() */
  mpf_t point[2];
  mpf_init2 (point[0], PREC); /* precision at least PREC bits (defined in julia.h) */
  mpf_init2 (point[1], PREC);

  /* radius_start: the original start of the zoom. */
  mpf_t radius_start[2];
  mpf_init2 (radius_start[0], PREC);
  mpf_init2 (radius_start[1], PREC);

  /* radius: the current radius at any given time, updated for every frame. */
  mpf_t radius[2];
  mpf_init2 (radius[0], PREC);
  mpf_init2 (radius[1], PREC);

  /* zoom_rate: the magnification from one frame to the next. */
  mpf_t zoom_rate;
  mpf_init2 (zoom_rate, PREC);

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size (MPI_COMM_WORLD, &processes);

  /* Tell me what the current precision is */
  if (my_rank == 0)
    printf ("Current precision is %d\n", PREC);

  /* Read data from file params.dat:
   * point: these are the real (x) and imaginary (y) values of z.
   * radius_start: the starting radii (x and y) for the zoom.
   * start_image: the number of the first image to save.  Used to
   *              calculate zoom level for the image.
   * end_image: the number of the image to stop the program at.
   * width: the width in pixels of the image (the x-resolution).
   * height: the height of the image in pixels.
   * maxiter: the maximum number of iterations to perform on each pixel.
   * zoom_rate: the rate at which to zoom from one frame to next.
   * image: the name of the image file produced, without an extension.*/
  getParams(argv, point, radius_start, &start_image, &end_image, &width, &height, &maxiter, &zoom_rate, image);

  /* Some calulations:
   *x[0] (x_min) = point[0] - radius[0];
   *x[1] (x_max) = point[0] + radius[0];
   *y[0] (y_min) = point[1] - radius[1];
   *y[1] (y_max) = point[1] + radius[1];
   *x[2] = {x_min, x_max};
   *y[2] = {y_min, y_max};
   */

  mpf_t x[2]; //the real max and min coordinates.
  mpf_t y[2]; //the imaginary max and min coordinates.

  mpf_init2 (x[0], PREC);
  mpf_init2 (x[1], PREC);
  mpf_init2 (y[0], PREC);
  mpf_init2 (y[1], PREC);

  mpf_t zoom;
  mpf_init2 (zoom, PREC);

  /* Each process calculates the number of the image to work on. */
  int current_image = my_rank + start_image;

  /* calculate zoom level of the current image being created. */
  mpf_pow_ui (zoom, zoom_rate, current_image);

  /* calculate the x and y radii of the image. */
  mpf_div (radius[0], radius_start[0], zoom);
  mpf_div (radius[1], radius_start[1], zoom);

  /* calculate x[0] (xmin) and y[0] (ymin) for the current image.*/
  mpf_sub (x[0], point[0], radius[0]);
  mpf_add (x[1], point[0], radius[0]);
  mpf_sub (y[0], point[1], radius[1]);
  mpf_add (y[1], point[1], radius[1]);

  double my_time = 0;
  int num_images = 0;

  /* Each processor processes one image at a time
   * until there are no more images to process */
  while (current_image <= end_image) {
    float *iterations;
    iterations = (float*) malloc (sizeof (float) * width * height);
    assert(iterations);

    /* call julia */
    double to = MPI_Wtime ();
    julia (width, height, x, y, maxiter, my_rank, iterations, processes);
    my_time += (MPI_Wtime () - to);

    /* save image-number.bmp */
    int len;
    char *image1;
    len = strlen(image);

    image1 = malloc(len + 11);  // -XXXXX.bmp + 0
    strncpy(image1, image, len);

    snprintf (image1, len + 11, "%s-%05d.bmp", image, current_image);
    saveBMP(image1, iterations, width, height);
   //saveIterations(image1, iteration, width, height);

    /* print image name and data */
    gmp_printf ("%s: radius %Fe, zoom level %Fe\n", image1, radius[0], zoom);
    free (image1);
    free (iterations);

    num_images = current_image - start_image + 1;
    current_image+=processes;

    /* calculate new zoom level */
    mpf_pow_ui (zoom, zoom_rate, current_image);

    /* calculate new x and y for the next image. */
    mpf_div (radius[0], radius_start[0], zoom);
    mpf_div (radius[1], radius_start[1], zoom);

    /* calculate xmin and ymin for the next image. */
    mpf_sub (x[0], point[0], radius[0]);
    mpf_add (x[1], point[0], radius[0]);
    mpf_sub (y[0], point[1], radius[1]);
    mpf_add (y[1], point[1], radius[1]);
  }

  mpf_clear (point[0]);
  mpf_clear (point[1]);
  mpf_clear (radius[0]);
  mpf_clear (radius[1]);
  mpf_clear (radius_start[0]);
  mpf_clear (radius_start[1]);
  mpf_clear (zoom_rate);
  mpf_clear (zoom);
  mpf_clear (x[0]);
  mpf_clear (x[1]);
  mpf_clear (y[0]);
  mpf_clear (y[1]);

  /* Collect some timing data */
  double total_compute_time = 0;
  double total_parallel_compute_time = 0;
  int total_images = 0;
  MPI_Reduce (&my_time, &total_compute_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce (&my_time, &total_parallel_compute_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce (&num_images, &total_images, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    double compute_minutes = total_compute_time / 60;
    double parallel_minutes = total_parallel_compute_time / 60;
    printf ("Total compute time for %d images:  %.2lf seconds (%.2lf minutes)\n", total_images, total_compute_time, compute_minutes);
    printf ("Total parallel time for %d images: %.2lf seconds (%.2lf minutes)\n", total_images, total_parallel_compute_time, parallel_minutes);
  }

  MPI_Finalize ();
  return 0;
}
