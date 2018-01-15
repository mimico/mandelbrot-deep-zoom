#include <stdlib.h> /* exit () */
#include <stdio.h>
#include "julia.h"
#include "gmp.h"

void getParams(char     **argv,
               mpf_t    *point,
               mpf_t    *radius,
               int      *start_image,
               int      *end_image,
               int      *width,
               int      *height,
               int      *maxiter,
               mpf_t    *zoom_rate,
               char     *image)
{
  FILE *fp;
  char *filename = argv[1];
  fp = fopen (filename, "r");

  /* if no file by that name, give error and exit program */
  if (fp == NULL) {
    fprintf(stderr, "Can't open input file %s!\n", filename);
    exit(1);
  }

  /* Read the file */
  gmp_fscanf (fp, "%Ff", point[0]);
  gmp_fscanf (fp, "%Ff", point[1]);
  gmp_fscanf (fp, "%Ff", radius[0]);
  gmp_fscanf (fp, "%Ff", radius[1]);
  fscanf (fp, "%d", start_image);
  fscanf (fp, "%d", end_image);
  fscanf (fp, "%d", width);
  fscanf (fp, "%d", height);
  fscanf (fp, "%d", maxiter);
  gmp_fscanf (fp, "%Ff", zoom_rate);
  fscanf (fp, "%s", image);

  fclose (fp);
  return;
}
