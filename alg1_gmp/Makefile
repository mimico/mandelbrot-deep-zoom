CC = mpicc
CFLAGS=-g -Wall -O3 -fopenmp

OBJS =  main.o julia.o julia_row.o savebmp.o getparams.o

video: $(OBJS)
	$(CC) $(CFLAGS) -o video $(OBJS) -lm -lgmp

$(OBJS): julia.h
run2: video
	mpirun -np 2 ./video params.dat

run4: video
	mpirun -np 4 ./video params.dat

run8: video
	mpirun -np 8 ./video params.dat

run16: video
	mpirun -np 16 ./video params.dat

run32: video
	mpirun -np 32 ./video params.dat

run64: video
	mpirun -np 64 ./video params.dat

run128: video
	mpirun -np 128 ./video params.dat

run1024: video
	mpirun -np 1024 ./video params.dat

clean:
	rm -rf $(OBJS) video *~ *.bak *.bmp
