#include <stdlib.h> /* exit () */
#include <stdio.h>
#include "julia.h"

void getParams(char     **argv,
               double    *point,
               double    *radius,
               int      *start_image,
               int      *end_image,
               int      *width,
               int      *height,
               int      *maxiter,
               double    *zoom_rate,
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
  fscanf (fp, "%lf", &point[0]);
  fscanf (fp, "%lf", &point[1]);
  fscanf (fp, "%lf", &radius[0]);
  fscanf (fp, "%lf", &radius[1]);
  fscanf (fp, "%d", start_image);
  fscanf (fp, "%d", end_image);
  fscanf (fp, "%d", width);
  fscanf (fp, "%d", height);
  fscanf (fp, "%d", maxiter);
  fscanf (fp, "%lf", zoom_rate);
  fscanf (fp, "%s", image);

  fclose (fp);
  return;
}
