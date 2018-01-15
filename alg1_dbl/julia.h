#define thread_count 4   /* for openMP num_threads */

struct worker_stats
{
  double my_time;
};

void julia_row           (int           width,
                         double        xgap,
                         double        ygap,
                         const double   x0,
                         const double   y0,
                         int           row,
                         int           maxiter,
                         float          *iterations_row);

void julia              (int           width,
                         int           height,
                         double        *x,               /* xmin and xmax */
                         double        *y,               /* ymin and ymax */
                         int           maxiter,
                         int           my_rank,
                         float          *iterations,
                         int           processes);


void getParams          (char        **argv,
                         double        *point,
                         double        *radius,
                         int          *start_image,
                         int          *end_image,
                         int          *width,
                         int          *height,
                         int          *maxiter,
                         double        *zoom_rate,
                         char         *image);

void saveBMP            (char         *filename,
                         float        *result,
                         int           width,
                         int           height);
