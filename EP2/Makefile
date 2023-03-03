OUTPUT=mandelbrot

IMAGE=.ppm

CC=gcc
MCC=mpicc
CC_OPT=-std=c11

CC_OMP=-fopenmp
CC_PTH=-pthread

.PHONY: all
all: $(OUTPUT)_omp $(OUTPUT)_pth $(OUTPUT)_seq $(OUTPUT)_ompi $(OUTPUT)_ompi+omp $(OUTPUT)_ompi+pth

$(OUTPUT)_omp: $(OUTPUT)_omp.c
	$(CC) -o $(OUTPUT)_omp $(CC_OPT) $(CC_OMP) $(OUTPUT)_omp.c

$(OUTPUT)_pth: $(OUTPUT)_pth.c
	$(CC) -o $(OUTPUT)_pth $(CC_OPT) $(CC_PTH) $(OUTPUT)_pth.c

$(OUTPUT)_seq: $(OUTPUT)_seq.c
	$(CC) -o $(OUTPUT)_seq $(CC_OPT) $(OUTPUT)_seq.c

$(OUTPUT)_ompi: $(OUTPUT)_ompi.c
	$(MCC) -o $(OUTPUT)_ompi $(CC_OPT) $(OUTPUT)_ompi.c

$(OUTPUT)_ompi+omp: $(OUTPUT)_ompi+omp.c
	$(MCC) -o $(OUTPUT)_ompi+omp $(CC_OPT) $(CC_OMP) $(OUTPUT)_ompi+omp.c

$(OUTPUT)_ompi+pth: $(OUTPUT)_ompi+pth.c
	$(MCC) -o $(OUTPUT)_ompi+pth $(CC_OPT) $(CC_PTH) $(OUTPUT)_ompi+pth.c

.PHONY: clean
clean:
	rm $(OUTPUT)_omp $(OUTPUT)_pth $(OUTPUT)_seq $(OUTPUT)_ompi $(OUTPUT)_ompi+omp $(OUTPUT)_ompi+pth *$(IMAGE)
