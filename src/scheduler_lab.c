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

// check le repo python : 
// https://github.com/remzi-arpacidusseau/ostep-homework/blob/master/cpu-sched/scheduler.py
// 
void job_list(options_t *options) {
    jobs_t jobs;

    while (options.jobs != NULL) { 
        jobs->id = options->jobs;
        if (options.policy == RR) {
            jobs->remaining = options->runtime;
        }
        jobs->runtime = options->max_runtime;
        jobs->response = 0.0;
        jobs->turnaround = 0.0;
        jobs->wait = 0.0;
    }
}

char *strupcase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32;
        }
    }
    return str;
}

void helper(options_t *options, char *value) {
    char *capitalized_value = strupcase(value);

    if (strcmp("FIFO", capitalized_value) == 0) {
        options->policy = FIFO;
    } else if (strcmp("SJF", capitalized_value) == 0) {
        options->policy = SJF;
    } else if (strcmp("RR", capitalized_value) == 0) {
        options->policy = RR;
    } else {
        printf("Unknown Policy. Enter : FIFO, SJF or RR\n");
        options->policy = UNKNOWN;
    }
}

void run_flag(options_t *options, char flag, char *value) {
    switch (tolower(flag)) {
        case 'p':
            helper(options, value);
            break;
        case 's':
            if (atoi(value) == 1) {
                options->seed = 1;
            } else if (atoi(value) > 1) {
                fprintf(stderr, "Wrong input. Seed is either 1 or 0.\n");
            }
            break;
        case 'j':
            if (atoi(value) > 0) {
                options->jobs = atoi(value);
            }
            if (atoi(value) > MAX_JOBS) {
                fprintf(stderr, "Value is too big. Was set to Max Jobs (100).\n");
                options->jobs = MAX_JOBS;
            }
            break;
        case 'm':
            int maxlen = 1 + rand() % MAX_JOBS;
            if (maxlen <= 0) {
                fprintf(stderr, "Error generating runtime...\n");
            }
            options->max_runtime = maxlen;
            break;
        case 'q':
            if (atoi(value) > 1) {
                options->quantum = atoi(value);
            } else if (atoi(value) <= 0) { 
                fprintf(stderr, "Error generating length time slice.\n");
            }
            break;
        case 'c':
            if (strcmp("True", value) == 0 || strcmp("true", value) == 0) {
                options->solve = true;
            } else if (strcmp("false", value) == 0 || strcmp("False", value) == 0) {
                options->solve = false;
            }
            else {
                fprintf(stderr, "Error. write either {true} or {false}.\n");
            }
            break;
        default:
            break;    
    }
}

void parse_args(int argc, char **argv) {
    options_t options;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'c') {
            options.solve = true;
        } else if (argv[i][0] == '-') {
            char flag = argv[i][1];
            char *value = argv[i+1];
            run_flag(&options, flag, value);
            i++;
        }
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    parse_args(argc, argv);
    
    return 0;
}