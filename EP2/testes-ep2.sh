#!/bin/bash

for name in 'mandelbrot_ompi'
do
  for num_process in 1 8 16
  do
    for i in `seq 1 10`
    do
      printf "$name\ti: $i\tnum_process: $num_process\tnum_threads: 1\ttime: " >> testes-ep2.log
      mpirun --host localhost:$num_process $name -2.5 1.5 -2.0 2.0 4096 >> testes-ep2.log
    done
  done
done

for name in 'mandelbrot_ompi+pth' 'mandelbrot_ompi+omp'
do
  for num_process in 1 8 16
  do
    for num_threads in 1 2 4 8 16 32
    do
      for i in `seq 1 10`
      do
        printf "$name\ti: $i\tnum_process: $num_process\tnum_threads: $num_threads\ttime: " >> testes-ep2.log
        mpirun --host localhost:$num_process $name -2.5 1.5 -2.0 2.0 4096 $num_threads >> testes-ep2.log
      done
    done
  done
done
