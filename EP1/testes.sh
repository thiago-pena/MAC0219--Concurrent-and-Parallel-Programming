#!/bin/bash

for name in 'mandelbrot_seq'
do
  for num_threads in 1
  do
    for image_size in 16 32 64 128 256 512 1024 2048 4096 8192
    do
      for i in `seq 1 10`
      do
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: "  >> full.log
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: " >> seahorse.log
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: " >> elephant.log
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: " >> triple_spiral.log
        ./$name -2.5 1.5 -2.0 2.0 $image_size $num_threads >> full.log
        ./$name -0.8 -0.7 0.05 0.15 $image_size $num_threads >> seahorse.log
        ./$name 0.175 0.375 -0.1 0.1 $image_size $num_threads >> elephant.log
        ./$name -0.188 -0.012 0.554 0.754 $image_size $num_threads >> triple_spiral.log
        
      done
    done
  done
done

for name in 'mandelbrot_pth' 'mandelbrot_omp'
do
  for num_threads in 1 2 4 8 16 32
  do
    for image_size in 16 32 64 128 256 512 1024 2048 4096 8192
    do
      for i in `seq 1 10`
      do
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: "  >> full.log
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: " >> seahorse.log
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: " >> elephant.log
        printf "$name\ti: $i\tnum_threads: $num_threads\timage_size: $image_size\ttime: " >> triple_spiral.log
        ./$name -2.5 1.5 -2.0 2.0 $image_size $num_threads >> full.log
        ./$name -0.8 -0.7 0.05 0.15 $image_size $num_threads >> seahorse.log
        ./$name 0.175 0.375 -0.1 0.1 $image_size $num_threads >> elephant.log
        ./$name -0.188 -0.012 0.554 0.754 $image_size $num_threads >> triple_spiral.log
        
      done
    done
  done
done
