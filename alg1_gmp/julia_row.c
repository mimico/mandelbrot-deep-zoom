#include <assert.h>
#include "julia.h"
#include "math.h"

void julia_row (int              width,
               double           xgap,
               double           ygap,
               const mpf_t      x0,
               const mpf_t      y0,
               int              row,
               int              maxiter,
               float           *iterations_row)
{
  int column;

  mpf_t temp;
  mpf_t xi, yi;
  mpf_t savex, savey;
  mpf_t xi_2_plus_yi_2;
  mpf_t xi_squared, yi_squared;

  mpf_init2(xi, PREC);
  mpf_init2(yi, PREC);
  mpf_init2 (temp, PREC);
  mpf_init2(savex, PREC);
  mpf_init2(savey, PREC);
  mpf_init2 (xi_squared, PREC);
  mpf_init2 (yi_squared, PREC);
  mpf_init2 (xi_2_plus_yi_2, PREC);

  for (column = 0; column < width; column++) {
      /* pixel to coordinates:
       * xi = x0 + column * xgap;
       * yi = y0 + row * ygap;
      */
      mpf_set_d (xi, column * xgap);
      mpf_add (xi, x0, xi);
      mpf_set_d (yi, row * ygap);
      mpf_add (yi, y0, yi);

      mpf_set (savex, xi);
      mpf_set (savey, yi);

      mpf_mul (xi_squared, xi, xi);
      mpf_mul (yi_squared, yi, yi);

      int count;
      double radius = 0.0;
      double escape_radius = 100;

      for (count = 0; radius <= escape_radius && count < maxiter; count++) {
        mpf_mul_2exp (temp, xi, 1); /* temp = xi * 2 */

        /* xi = xi * xi - yi * yi + savex; */
        mpf_sub (xi, xi_squared, yi_squared);
        mpf_add (xi, xi, savex);

        /* yi = temp * yi + savey; */
        mpf_mul (temp, temp, yi);
        mpf_add (yi, temp, savey);

        mpf_mul (xi_squared, xi, xi);
        mpf_mul (yi_squared, yi, yi);
        mpf_add (xi_2_plus_yi_2, xi_squared, yi_squared);

        radius = mpf_get_d (xi_2_plus_yi_2);

        /* From the mpf_get_d docs:
         * If the exponent in op is too big or too small
         * to fit a double then the result is system dependent.
         * For too big an infinity is returned when available. For too
         * small 0.0 is normally returned.
         * Hardware overflow, underflow and denorm traps may or may not occur.
         * occur.
         *
         * On my laptop and on mpihost01 (didn't test this on Blue
         * Gene), for too small it actually returning inf.
         * This can be visualised  when trying to produce the Julia unit
         * circle point (0,0) with radius 2.  Points which should be in
         * the Julia set escape (are inf) at 63 iterations, when at 62
         * iterations, they were very very small.  The following
         * statement checks for implausibly large values and sets radius
         * to 0.0 in order to catche this case.
         */
        if (radius > 1e100)
          radius = 0.0;
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

  mpf_clear (xi);
  mpf_clear (yi);
  mpf_clear (temp);
  mpf_clear (savex);
  mpf_clear (savey);
  mpf_clear (xi_squared);
  mpf_clear (yi_squared);
  mpf_clear (xi_2_plus_yi_2);

  return;
}
