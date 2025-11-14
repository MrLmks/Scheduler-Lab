#ifndef SCHEDULER_LAB_
#define SCHEDULER_LAB_

#define MAX_JOBS 20

typedef struct job_s {
    int id;
    float runtime;
    float remaining;
    float response;
    float turnaround;
    float wait;
} job_t;

typedef enum{FIFO, SJF, RR, UNKNOWN} policy_t;

typedef struct options_s {
    policy_t policy;
    bool solve;
    job_t *jobs_params;
    float quantum;
    int max_runtime;
    bool use_seed;
    unsigned int seed_value;
    int jobs;
    char jlist[256];
    bool use_random;
} options_t;


void show_jobs(options_t *options);
int count_jobs(const char *token);
void extract_job_list(options_t *options);
void job_list_random(options_t *options);
void show_jobs_fifo_sjf(options_t *options);
void show_round_robin(options_t *options);
void print_help();

#endif