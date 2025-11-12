#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "../headers/scheduler_lab.h"


#define MAX_JOBS 100


void show_round_robin(options_t *options) {
    for (int i = 0; i < options->jobs; i++) {
        
    }
}

void swap(float *a, float *b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void quick_sort(options_t *options, float *values, int start, int end) {
    int i = start;
    int pivot_index = start + (end - start) / 2;
    int j = end - 1;
    float pivot = values[pivot_index];

    if (start >= end - 1) {
        return;
    }

    while (i < j) {
        if (values[i] < pivot) {
            i++;
        }
        if (values[j] > pivot) {
            j--;
        }
        if (i < j) {
            swap(values + i, values + j);
        }
        if (i >= j) {
            break;
        }
    }
    quick_sort(options, values, start, j);
    quick_sort(options, values, j + 1, end);
}

void show_jobs_fifo_sjf(options_t *options) {
    float *runtime_values = malloc(sizeof(float) * (options->jobs));
    int j = 0;

    for (int i = 0; i < options->jobs; i++) {
        runtime_values[j] = options->jobs_params[i].runtime;
        j++;
    }

    quick_sort(options, runtime_values, 0, options->jobs);

    for (int i = 0; i < options->jobs; i++) {
        printf("jobs[%d] = %f\n", options->jobs_params[i].id, runtime_values[i]);
    }
}

void show_jobs(options_t *options) {
    int i = 0;

    while (i < options->jobs) {
        printf("job[%d] = %f\n", options->jobs_params[i].id, options->jobs_params[i].runtime);
        i++;
    }
}

int count_jobs(char *token) {
    int count = 0;
    int len = strlen(token);

    if (token[0] >= '0' && token[0] <= '9') {
            count += 1;
    }
    for (int i = 0; i < len - 1; i++) {
        if (token[i] == ',' && token[i+1] >= '0' && token[i+1] <= '9') {
            count += 1;
        }
    }
    return count;
}

void init_jobs_round_robin(options_t *options) {
    for (int i = 0; i < options->jobs; i++) {
        options->jobs_params[i].wait = 0.0;
        options->jobs_params[i].response = 0.0;
        options->jobs_params[i].turnaround =  options->jobs_params[i].wait +  options->jobs_params[i].runtime;
        options->jobs_params[i].remaining = options->jobs_params[i].runtime;
    }
}

void extract_job_list(options_t *options) {
    char tmp[MAX_JOBS];
    strcpy(tmp, options->jlist);
    char *token = strtok(tmp, ","); 
    int count = count_jobs(options->jlist);
    options->jobs_params = malloc(sizeof(job_t) * (count));
    int i = 0;

    while (token != NULL) {
        options->jobs_params[i].id = i;
        options->jobs_params[i].runtime = atof(token);
        options->jobs_params[i].wait = 0.0;
        options->jobs_params[i].response = 0.0;
        options->jobs_params[i].turnaround =  options->jobs_params[i].wait +  options->jobs_params[i].runtime;
        options->jobs_params[i].remaining = 0;
        i++;
        token = strtok(NULL, ",");
    }
}

void job_list_random(options_t *options) {
    options->jobs = 1 + rand() % MAX_JOBS;
    options->jobs_params = malloc(sizeof(job_t) * (options->jobs));

    for (int i = 0; i < options->jobs; i++) {
        options->jobs_params[i].id = i;
        options->jobs_params[i].runtime = 1.0 + (((float)rand() / (float)RAND_MAX) * (options->max_runtime - 1.0));
        options->jobs_params[i].wait = 0.0;
        options->jobs_params[i].response = 0.0;
        options->jobs_params[i].turnaround = options->jobs_params[i].wait + options->jobs_params[i].runtime;
        options->jobs_params[i].remaining = 0;
    }
}