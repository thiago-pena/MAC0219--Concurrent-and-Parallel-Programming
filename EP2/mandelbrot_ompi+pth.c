#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "mpi.h"

#define MASTER 0
#define DEBUG 0

double c_x_min;
double c_x_max;
double c_y_min;
double c_y_max;

double pixel_width;
double pixel_height;

int iteration_max = 200;

int image_size;
unsigned char **image_buffer;

int i_x_max;
int i_y_max;
int image_buffer_size;

int gradient_size = 16;
int colors[17][3] = {
    {66, 30, 15},
    {25, 7, 26},
    {9, 1, 47},
    {4, 4, 73},
    {0, 7, 100},
    {12, 44, 138},
    {24, 82, 177},
    {57, 125, 209},
    {134, 181, 229},
    {211, 236, 248},
    {241, 233, 191},
    {248, 201, 95},
    {255, 170, 0},
    {204, 128, 0},
    {153, 87, 0},
    {106, 52, 3},
    {16, 16, 16},
};

/* Variáveis globais adicionadas (PTHREADS) */
int num_threads;
typedef struct {
    int taskid;
    int thread_id;
    int start;
    int end;
} th_data;
typedef th_data *ThreadData;
ThreadData *thread_data;

/* Para uso do OMPI */
unsigned char *linear_buffer0;
unsigned char *linear_buffer1;
unsigned char *linear_buffer2;
int linear_buffer_size;

void linear_buffer_to_image_buffer() {
    for (int i = 0; i < linear_buffer_size; i++) {
        image_buffer[i][0] = linear_buffer0[i];
        image_buffer[i][1] = linear_buffer1[i];
        image_buffer[i][2] = linear_buffer2[i];
    }
};

void image_buffer_to_linear_buffer() {
    for (int i = 0; i < linear_buffer_size; i++) {
        linear_buffer0[i] = image_buffer[i][0];
        linear_buffer1[i] = image_buffer[i][1];
        linear_buffer2[i] = image_buffer[i][2];
    }
};

void print_image_buffer() {
    printf("\n\n[DEBUG] print_image_buffer\n");
    printf("x\ty\t\trgb\n");
    for (int i = 0; i < image_buffer_size; i++) {
        int x = i % i_y_max;
        int y = i / i_y_max;
        printf("%d\t%d\t\t%d\t%d\t%d\n", x, y, image_buffer[i][0], image_buffer[i][1], image_buffer[i][2]);
    }
};

void print_linear_buffer() {
    printf("\n\n[DEBUG] print_linear_buffer\n");
    printf("x\ty\t\trgb\n");
    for (int i = 0; i < linear_buffer_size; i++) {
        int x = i % i_y_max;
        int y = i / i_y_max;
        printf("%d\t%d\t\t%d\t%d\t%d\n", x, y, linear_buffer0[i], linear_buffer1[i], linear_buffer2[i]);
    }
};

/* Para medição do tempo */
int elapsedTime(struct timeval a, struct timeval b) {
    return ((b.tv_sec - a.tv_sec) * 1000000) + (b.tv_usec - a.tv_usec);
}

void allocate_image_buffer() {
    int rgb_size = 3;
    image_buffer = (unsigned char **)malloc(sizeof(unsigned char *) * image_buffer_size);
    linear_buffer0 = (unsigned char *)malloc(i_x_max * i_y_max * (sizeof(unsigned char *)));
    linear_buffer1 = (unsigned char *)malloc(i_x_max * i_y_max * (sizeof(unsigned char *)));
    linear_buffer2 = (unsigned char *)malloc(i_x_max * i_y_max * (sizeof(unsigned char *)));

    for (int i = 0; i < image_buffer_size; i++) {
        image_buffer[i] = (unsigned char *)malloc(sizeof(unsigned char) * rgb_size);
    };
};

void init(int argc, char *argv[]) {
    if (argc < 6) {
        printf("usage: ./mandelbrot_seq c_x_min c_x_max c_y_min c_y_max image_size\n");
        printf("examples with image_size = 11500:\n");
        printf("    Full Picture:         ./mandelbrot_seq -2.5 1.5 -2.0 2.0 11500\n");
        printf("    Seahorse Valley:      ./mandelbrot_seq -0.8 -0.7 0.05 0.15 11500\n");
        printf("    Elephant Valley:      ./mandelbrot_seq 0.175 0.375 -0.1 0.1 11500\n");
        printf("    Triple Spiral Valley: ./mandelbrot_seq -0.188 -0.012 0.554 0.754 11500\n");
        exit(0);
    } else {
        sscanf(argv[1], "%lf", &c_x_min);
        sscanf(argv[2], "%lf", &c_x_max);
        sscanf(argv[3], "%lf", &c_y_min);
        sscanf(argv[4], "%lf", &c_y_max);
        sscanf(argv[5], "%d", &image_size);
        sscanf(argv[6], "%d", &num_threads);

        i_x_max = image_size;
        i_y_max = image_size;
        image_buffer_size = image_size * image_size;
        linear_buffer_size = image_buffer_size;

        pixel_width = (c_x_max - c_x_min) / i_x_max;
        pixel_height = (c_y_max - c_y_min) / i_y_max;
    };
};

void update_rgb_buffer(int iteration, int x, int y) {
    int color;

    if (iteration == iteration_max) {
        linear_buffer0[i_y_max * y + x] = colors[gradient_size][0];
        linear_buffer1[i_y_max * y + x] = colors[gradient_size][1];
        linear_buffer2[i_y_max * y + x] = colors[gradient_size][2];
    } else {
        color = iteration % gradient_size;
        linear_buffer0[i_y_max * y + x] = colors[color][0];
        linear_buffer1[i_y_max * y + x] = colors[color][1];
        linear_buffer2[i_y_max * y + x] = colors[color][2];
    };
};

void write_to_file() {
    FILE *file;
    char *filename = "output.ppm";
    char *comment = "# ";

    int max_color_component_value = 255;

    linear_buffer_to_image_buffer();
    file = fopen(filename, "wb");

    fprintf(file, "P6\n %s\n %d\n %d\n %d\n", comment,
            i_x_max, i_y_max, max_color_component_value);
    for (int i = 0; i < image_buffer_size; i++) {
        fwrite(image_buffer[i], 1, 3, file);
    };

    fclose(file);
};

void *compute_mandelbrot_thread(void *a) {
    ThreadData data = (ThreadData)a;
    if (DEBUG) printf("taskid: %d, thread_id: %d, start: %d, end: %d\n",
                      data->taskid, data->thread_id, data->start, data->end);

    double z_x;
    double z_y;
    double z_x_squared;
    double z_y_squared;
    double escape_radius_squared = 4;

    int iteration;
    int i_x;
    int i_y;

    double c_x;
    double c_y;

    for (i_y = data->start; i_y < data->end; i_y++) {
        c_y = c_y_min + i_y * pixel_height;

        if (fabs(c_y) < pixel_height / 2) {
            c_y = 0.0;
        };

        for (i_x = 0; i_x < i_x_max; i_x++) {
            c_x = c_x_min + i_x * pixel_width;

            z_x = 0.0;
            z_y = 0.0;

            z_x_squared = 0.0;
            z_y_squared = 0.0;

            for (iteration = 0;
                 iteration < iteration_max &&
                 ((z_x_squared + z_y_squared) < escape_radius_squared);
                 iteration++) {
                z_y = 2 * z_x * z_y + c_y;
                z_x = z_x_squared - z_y_squared + c_x;

                z_x_squared = z_x * z_x;
                z_y_squared = z_y * z_y;
            };

            update_rgb_buffer(iteration, i_x, i_y);
        };
    };
};

void compute_mandelbrot(int start, int end, int taskid) {
    if (DEBUG) printf("Computing Mandelbrot in taskid %d (%d,%d)\n", taskid, start, end);
    int chunksize = (end - start) / num_threads;
    int leftover = (end - start) % num_threads;

    pthread_t ths[num_threads];

    int thread_start = start;
    int thread_end = start + chunksize + leftover;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i]->thread_id = i;
        thread_data[i]->start = thread_start;
        thread_data[i]->end = thread_end;
        thread_data[i]->taskid = taskid;

        if (pthread_create(&ths[i], NULL, compute_mandelbrot_thread, thread_data[i])) {
            printf("\n ERROR creating thread %d\n", i);
            exit(1);
        }
        thread_start += chunksize;
        thread_end += chunksize;
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(ths[i], NULL);
    }
};

int main(int argc, char *argv[]) {
    struct timeval time_start, time_end;

    /***** Initializations *****/
    int numtasks, taskid, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;

    init(argc, argv);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    int chunksize = i_y_max / numtasks;
    int leftover = i_y_max % numtasks;
    int offset;
    int tag0 = 0;
    int tag1 = 1;
    int tag2 = 2;
    int tag_offset_1 = 100;
    int tag_offset_2 = 200;

    thread_data = malloc(num_threads * sizeof(thread_data));
    for (int i = 0; i < num_threads; i++) {
        thread_data[i] = malloc(sizeof(th_data));
    }

    allocate_image_buffer();
    /***** Master task only ******/
    if (taskid == MASTER) {
        gettimeofday(&time_start, NULL);
        /* Send each task its portion - master keeps 1st part plus leftover elements */
        int offset = chunksize + leftover;
        for (int dest = 1; dest < numtasks; dest++) {
            MPI_Send(&offset, 1, MPI_INT, dest, tag_offset_1, MPI_COMM_WORLD);
            offset = offset + chunksize;
        }

        /* Master does its part of the work */
        offset = 0;

        int start = offset;
        int end = chunksize + leftover;
        if (DEBUG) {
            printf(">>> MASTER %d | start: %d end: %d\n", taskid, start, end);
            // print_linear_buffer();
        }
        compute_mandelbrot(start, end, taskid);

        /* Wait to receive results from each task */
        for (int i = 1; i < numtasks; i++) {
            int source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, tag_offset_2, MPI_COMM_WORLD, &status);
            MPI_Recv(&linear_buffer0[i_y_max * offset], i_y_max * chunksize, MPI_UNSIGNED_CHAR, source, tag0, MPI_COMM_WORLD, &status);
            MPI_Recv(&linear_buffer1[i_y_max * offset], i_y_max * chunksize, MPI_UNSIGNED_CHAR, source, tag1, MPI_COMM_WORLD, &status);
            MPI_Recv(&linear_buffer2[i_y_max * offset], i_y_max * chunksize, MPI_UNSIGNED_CHAR, source, tag2, MPI_COMM_WORLD, &status);

            if (DEBUG) {
                int start = offset;
                int end = start + chunksize;
                printf("\t MASTER recebe array de %d (%d, %d)\n", numtasks, start, end);
                // print_linear_buffer();
            }
        }
        linear_buffer_to_image_buffer();
        if (DEBUG) print_image_buffer();
        write_to_file();
        gettimeofday(&time_end, NULL);
        printf("%d microsegundos\n", elapsedTime(time_start, time_end));
    } else {
        /* Receive my portion of array from the master task */
        int source = MASTER;
        MPI_Recv(&offset, 1, MPI_INT, source, tag_offset_1, MPI_COMM_WORLD, &status);
        /* Do my part of the work */
        int start = offset;
        int end = start + chunksize;
        if (DEBUG) printf(">>> taskid %d | start: %d end: %d\n", taskid, start, end);
        compute_mandelbrot(start, end, taskid);

        /* Send my results back to the master task */
        int dest = MASTER;
        MPI_Send(&offset, 1, MPI_INT, dest, tag_offset_2, MPI_COMM_WORLD);
        MPI_Send(&linear_buffer0[i_y_max * offset], i_y_max * chunksize, MPI_UNSIGNED_CHAR, MASTER, tag0, MPI_COMM_WORLD);
        MPI_Send(&linear_buffer1[i_y_max * offset], i_y_max * chunksize, MPI_UNSIGNED_CHAR, MASTER, tag1, MPI_COMM_WORLD);
        MPI_Send(&linear_buffer2[i_y_max * offset], i_y_max * chunksize, MPI_UNSIGNED_CHAR, MASTER, tag2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
};
