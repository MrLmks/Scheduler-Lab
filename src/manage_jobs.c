#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "../headers/scheduler_lab.h"

#define MAX_JOBS 20

bool has_jobs_remaining(float *runtime, options_t *options) {
    for (int i = 0; i < options->jobs; i++) {
        if (runtime[i] > 0) {
            return true;
        }
    }
    return false;
}

float *copy_runtimes(options_t *options) {
    float *runtime = malloc(sizeof(float) * options->jobs);
    int j = 0;

    if (runtime == NULL) {
        return NULL;
    }
    for (int i = 0; i < options->jobs; i++) {
        runtime[j] = options->jobs_params[i].runtime;
        j++;
    }

    return runtime;
}

float *init_to_zero(options_t *options) {
    float *values = malloc(sizeof(float) * options->jobs);

    if (values == NULL) {
        return NULL;
    }
    for (int i = 0; i < options->jobs; i++) {
        values[i] = 0.0;
    }
    return values;
}

void display_stats(options_t *options, float *wait_time, bool *job_done) {
    printf("Final statistics : \n");

    for (int i = 0; i < options->jobs; i++) {
        printf("Job[%d] - Response: %.2f - Turnaround: %.2f - Wait: %.2f\n", options->jobs_params[i].id,
            options->jobs_params[i].response, options->jobs_params[i].turnaround, wait_time[i]);
        if (job_done[i] == true) {
            printf("Done in %.2f seconds.\n", options->jobs_params[i].runtime);
        }
    }
}

bool *jobs_done(options_t *options) {
    bool *jobs = malloc(sizeof(bool) * options->jobs);

    for (int i = 0; i < options->jobs; i++) {
        jobs[i] = false;
    }
    return jobs;
}

void show_round_robin(options_t *options) {
    float *runtimes = copy_runtimes(options);
    float *wait_time = init_to_zero(options);
    float *last_run = init_to_zero(options);
    float global_time = 0.0;
    options->quantum = 2.0;
    int i = 0;
    bool *job_done = jobs_done(options);

    while (has_jobs_remaining(runtimes, options)) {
        if (runtimes[i] > 0) {
            wait_time[i] += (global_time - last_run[i]);
            if (options->jobs_params[i].response == 0.0)
                options->jobs_params[i].response = global_time;
            float time = (runtimes[i] < options->quantum) ? runtimes[i] : options->quantum;
            runtimes[i] -= time;
            global_time += time;
            last_run[i] = global_time;
        }
        if (runtimes[i] <= 0 && job_done[i] == false) {
            job_done[i] = true;
            options->jobs_params[i].turnaround = global_time;
        }
        i++;
        if (i >= options->jobs) {
            i = 0;
        }
    }
    display_stats(options, wait_time, job_done);

    free(runtimes);
    free(wait_time);
    free(last_run);
    free(job_done);
}

void swap(job_t *a, job_t *b) {
    job_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void quick_sort(job_t *jobs, int start, int end) {
    int i = start;
    int pivot_index = start + (end - start) / 2;
    int j = end - 1;
    float pivot = jobs[pivot_index].runtime;

    if (start >= end - 1) {
        return;
    }
    while (i < j) {
        if (jobs[i].runtime < pivot) {
            i++;
        }
        if (jobs[j].runtime > pivot) {
            j--;
        }
        if (i < j) {
            swap(&jobs[i], &jobs[j]);
        }
        if (i >= j) {
            break;
        }
    }
    quick_sort(jobs, start, j);
    quick_sort(jobs, j + 1, end);
}

void show_jobs_fifo_sjf(options_t *options) {
    quick_sort(options->jobs_params, 0, options->jobs);

    for (int i = 0; i < options->jobs; i++) {
        printf("Job[%d] = %f\n", i + 1, options->jobs_params[i].runtime);
    }
}

void show_jobs(options_t *options) {
    int i = 0;

    while (i < options->jobs) {
        printf("job[%d] = %f\n", i+1, options->jobs_params[i].runtime);
        i++;
    }
}

int count_jobs(const char *token) {
    int count = 0;
    const int len = strlen(token);

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
        options->jobs_params[i].turnaround =  0.0;
        options->jobs_params[i].remaining = 0.0;
    }
}

void extract_job_list(options_t *options) {
    int len = strlen(options->jlist) + 1;
    char tmp[len];
    strcpy(tmp, options->jlist);
    char *token = strtok(tmp, ",");
    int count = count_jobs(options->jlist);
    options->jobs = count;
    options->jobs_params = malloc(sizeof(job_t) * (count));
    int i = 0;

    while (token != NULL) {
        options->jobs_params[i].id = i;
        options->jobs_params[i].runtime = strtod(token, NULL);
        options->jobs_params[i].wait = 0.0;
        options->jobs_params[i].response = 0.0;
        options->jobs_params[i].turnaround =  0.0;
        options->jobs_params[i].remaining = options->jobs_params[i].runtime;
        i++;
        token = strtok(NULL, ",");
    }
}

void job_list_random(options_t *options) {
    if (options->jobs == 0) {
        options->jobs = 1 + rand() % MAX_JOBS;
    }
    options->jobs_params = malloc(sizeof(job_t) * (options->jobs));

    for (int i = 0; i < options->jobs; i++) {
        options->jobs_params[i].id = i;
        options->jobs_params[i].runtime = 1.0 + (((float)rand() / (float)RAND_MAX) * (options->max_runtime - 1.0));
        options->jobs_params[i].wait = 0.0;
        options->jobs_params[i].response = 0.0;
        options->jobs_params[i].turnaround = 0.0;
        options->jobs_params[i].remaining = options->jobs_params[i].runtime;
    }
}