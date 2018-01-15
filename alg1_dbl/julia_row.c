#include <assert.h>
#include "julia.h"
#include "math.h"

void julia_row (int              width,
               double           xgap,
               double           ygap,
               const double      x0,
               const double      y0,
               int              row,
               int              maxiter,
               float           *iterations_row)
{
  int column;

  double xi, yi;
  double savex, savey;
  double xi_squared, yi_squared;

  for (column = 0; column < width; column++) {
      /* pixel to coordinates:
       * xi = x0 + column * xgap;
       * yi = y0 + row * ygap;
      */
      xi = column * xgap;
      xi = x0 + xi;
      yi = row * ygap;
      yi = y0 + yi;

      savex = xi;
      savey = yi;

      xi_squared = xi * xi;
      yi_squared = yi * yi;

      int count;
      double radius = 0.0;
      double escape_radius = 100;

      for (count = 0; radius <= escape_radius && count < maxiter; count++) {
        double temp = xi * 2;
        /* xi = xi * xi - yi * yi + savex; */
        xi = xi_squared - yi_squared + savex;

        yi = temp * yi + savey;

        xi_squared = xi * xi;
        yi_squared = yi * yi;

        radius = xi_squared + yi_squared;
      }

      /* If radius <= escape_radius, we have hit maxiter.
       * The point is likely in the set. */
      if (radius <= escape_radius) {
        assert(count==maxiter);
        iterations_row[column] = 0;
      }
      else {
        /* radius > escape_radius. The point tends to infinity.
         * We recognize this at iteration number count */

        /* Normalized Iteration Count Algorithm:
         * Based on colouring formula in Ultrafractal, source:
         * http://en.wikibooks.org/wiki/Fractals/Iterations_in_the_complex_plane/Mandelbrot_set#Real_Escape_Time

         * precalculate some values:
           log(2.0) = 0.6931471805599453
           log(log(sqrt(escape_radius))) = log(log (10)) = 0.834032445247956 */
         iterations_row[column] = (float) count + 1.834032445247956 - log(log(log(sqrt(radius))) / 0.6931471805599453);

      }
    }

  return;
}
