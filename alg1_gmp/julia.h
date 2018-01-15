#include "gmp.h"

#define PREC  128        /* number of digits of precision */
#define thread_count 4   /* for openMP num_threads */

struct worker_stats
{
  double my_time;
};

void julia_row           (int           width,
                         double        xgap,
                         double        ygap,
                         const mpf_t   x0,
                         const mpf_t   y0,
                         int           row,
                         int           maxiter,
                         float          *iterations_row);

void julia              (int           width,
                         int           height,
                         mpf_t        *x,               /* xmin and xmax */
                         mpf_t        *y,               /* ymin and ymax */
                         int           maxiter,
                         int           my_rank,
                         float          *iterations,
                         int           processes);


void getParams          (char        **argv,
                         mpf_t        *point,
                         mpf_t        *radius,
                         int          *start_image,
                         int          *end_image,
                         int          *width,
                         int          *height,
                         int          *maxiter,
                         mpf_t        *zoom_rate,
                         char         *image);

void saveBMP            (char         *filename,
                         float        *result,
                         int           width,
                         int           height);
