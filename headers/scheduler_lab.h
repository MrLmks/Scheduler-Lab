
#include <stdbool.h>

typedef struct job_s {
    int id;
    int runtime;
    int remaining;
    float response;
    float turnaround;
    float wait;
} job_t;

typedef enum{FIFO, SJF, RR, UNKNOWN} policy_t;

typedef struct options_s {
    policy_t policy;
    bool solve;
    int quantum;
    int max_runtime;
    int seed;
    int jobs;
} options_t;