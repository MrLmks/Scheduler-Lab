
#include <stdbool.h>
#define MAX_JOBS 100

typedef struct job_s {
    int id;
    float runtime;
    int remaining;
    float response;
    float turnaround;
    float wait;
} job_t;

typedef enum{FIFO, SJF, RR, UNKNOWN} policy_t;

typedef struct options_s {
    policy_t policy;
    bool solve;
    job_t *jobs_params;
    int quantum;
    int max_runtime;
    bool use_seed;
    unsigned int seed_value;
    int jobs;
    char jlist[MAX_JOBS];
} options_t;


void show_jobs(options_t *options);
int count_jobs(char *token);
void extract_job_list(options_t *options);
void job_list_random(options_t *options);
void show_jobs_fifo_sjf(options_t *options);