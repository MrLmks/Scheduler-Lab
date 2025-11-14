#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "../headers/scheduler_lab.h"

#define MAX_JOBS 20
#define MAX_RUNTIME 20

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
        fprintf(stderr, "Unknown Policy. Enter : FIFO, SJF or RR\n");
        options->policy = UNKNOWN;
    }
}

bool check_joblist(const char *value) {
    for (int i = 0; value[i] != '\0'; i++) {
        if ((value[i] < '0' || value[i] > '9') && value[i] != ',') {
            return true;
        }
    }
    return false;
}

void run_flag(options_t *options, const char flag, char *value) {
    if (flag) {
        int maxlen = (int)strtol(value, NULL, 10);
        switch (tolower(flag)) {
        case 'p':
                helper(options, value);
                break;
        case 's':
                if ((int)strtol(value, NULL, 10) >= 1) {
                    options->use_seed = true;
                    options->seed_value = strtol(value, NULL, 10);
                } else if ((int)strtol(value, NULL, 10) <= 0) {
                    options->use_seed = false;
                    options->seed_value = 0;
                }
                break;
        case 'j':
                if ((int)strtol(value, NULL, 10) > 0) {
                    options->jobs = (int)strtol(value, NULL, 10);
                }
                if ((int)strtol(value, NULL, 10) > MAX_JOBS) {
                    fprintf(stderr, "Value is too big. Was set to MAX JOBS (= 20).\n");
                    options->jobs = MAX_JOBS;
                }
                break;
        case 'm':
                if (maxlen <= 0) {
                    fprintf(stderr, "Error generating runtime...\n");
                    return;
                }
                options->max_runtime = maxlen;
                break;
        case 'q':
                if ((int)strtol(value, NULL, 10) > 1) {
                    options->quantum = (int)strtol(value, NULL, 10);
                } else if (strtol(value, NULL, 10) <= 0) {
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
        case 'l':
                if (check_joblist(value)) {
                    fprintf(stderr, "Error. Write in this format : [12,34,43]\n");
                    return;
                }
                strncpy(options->jlist, value, MAX_JOBS);
                break;
        default:
            break;
        }
    }
}

bool check_flag(char flag) {
    if (flag == 'l' || flag == 'c' || flag == 'q' || flag == 'm'
        || flag == 'j' || flag == 's' || flag == 'p') {
            return true;
        }
    return false;
}

void parse_args(int argc, char **argv, options_t *options) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'c') {
            options->solve = true;
        }
        if (argv[i][0] == '-' && (argv[i][1] == 'h' || argv[i][1] == 'H')) {
            print_help();
            exit(0);
        }
        if (argv[i][0] == '-') {
            const char flag = argv[i][1];
            char *value = argv[i+1];
            if (!check_flag(flag)) {
                fprintf(stderr, "Insert proper flag. Do -h for help.\n");
                return;
            } if (check_flag(flag) && value == NULL) {
                fprintf(stderr, "Wrong value.\n");
                return;
            } if (check_flag(flag) && value != NULL) {
                run_flag(options, flag, value);
                i++;
                }
            }
    }
}

void print_help() {
    printf("Scheduler Lab - Usage:\n");
    printf("  -l <list>      : specify job list (comma-separated, e.g., 1,2,3)\n");
    printf("  -p <policy>    : select policy: fifo, sjf, rr\n");
    printf("  -s <seed>      : set random seed\n");
    printf("  -j <num_jobs>  : number of jobs (if no -l)\n");
    printf("  -m <max_time>  : maximum runtime for random jobs\n");
    printf("  -q <quantum>   : time slice for round-robin\n");
    printf("  -c <true|false>: solve flag\n");
    printf("  -h             : display this help message\n");
}

void check_options(options_t *options) {
    if (strlen(options->jlist) > 0) {
        options->use_random = false;
        extract_job_list(options);
    } else {
        options->use_random = true;
        job_list_random(options);
    }
    if (options->use_seed == true) {
        srand(options->seed_value);
    } else {
        srand(time(NULL));
    }
    if (options->policy == SJF) {
        show_jobs_fifo_sjf(options);
    }
    if (options->policy == FIFO) {
        show_jobs(options);
    }
    if (options->policy == RR) {
        show_round_robin(options);
    }
}

int main(const int argc, char **argv) {
    options_t options;
    options.max_runtime = MAX_RUNTIME;

    if (argc == 1) {
        srand(time(NULL));
        job_list_random(&options);
        show_jobs(&options);
    }
    if (argc > 1) {
        parse_args(argc, argv, &options);
        check_options(&options);
    }
    return 0;
}