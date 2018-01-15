#!/bin/sh
# @ job_name = dbl
# @ job_type = bluegene
# @ comment = "BGQ Job By Size"
# @ error = $(job_name).$(jobid).err
# @ output = $(job_name).$(jobid).out
# @ bg_size = 64
# @ wall_clock_limit = 1:00:00
# @ bg_connectivity = Torus
# @ queue

# Launch all BGQ jobs using runjob
runjob --np 1024 --ranks-per-node=16 --cwd=$PWD/ : $PWD/video dbl.dat
