#!/bin/bash

for name in 'mandelbrot_seq'
do
  for i in `seq 1 10`
  do
    printf "$name\ti: $i\tnum_process: 1\tnum_threads: 1\ttime: " >> testes-ep1.log
    ./$name -2.5 1.5 -2.0 2.0 4096 $num_threads >> testes-ep1.log
  done
done

for name in 'mandelbrot_pth' 'mandelbrot_omp'
do
  for num_threads in 32
  do
    for i in `seq 1 10`
    do
      printf "$name\ti: $i\tnum_process: 1\tnum_threads: $num_threads\ttime: " >> testes-ep1.log
      ./$name -2.5 1.5 -2.0 2.0 4096 $num_threads >> testes-ep1.log
    done
  done
done
