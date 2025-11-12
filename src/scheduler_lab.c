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

bool check_joblist(char *value) {
    for (int i = 0; value[i] != '\0'; i++) {
        if ((value[i] < '0' || value[i] > '9') && value[i] != ',') {
            return true;
        }
    }
    return false;
}

void run_flag(options_t *options, char flag, char *value) {
    if (flag) {
       switch (tolower(flag)) {
        case 'p':
            helper(options, value);
            break;
        case 's':
            if (atoi(value) >= 1) {
                options->use_seed = true;
                options->seed_value = atoi(value);
            } else if (atoi(value) <= 0) {
                options->use_seed = false;
                options->seed_value = 0;
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
                return;
            }
            options->max_runtime = maxlen;
            break;
        case 'q':
            if (atoi(value) > 1) {
                options->quantum = atoi(value);
            } else if (atoi(value) <= 0) { 
                fprintf(stderr, "Error generating length time slice.\n");
                return;
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
                return;
            }
            break;
        case 'l':
            if (check_joblist(value)) {
                fprintf(stderr, "Error. Write in this format : [12,34,43]\n");
                return;
            }
            strncpy(options->jlist, value, MAX_JOBS);
            extract_job_list(options);
            show_jobs(options);
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
        } else if (argv[i][0] == '-') {
            char flag = argv[i][1];
            char *value = argv[i+1];
            if (!check_flag(flag)) {
                fprintf(stderr, "Insert proper flag. Do -h for help.\n");
                return;
            } else if (check_flag(flag) && value == NULL) {
                fprintf(stderr, "Wrong value.\n");
                return;
            } else if (check_flag(flag) && value != NULL) {
                run_flag(options, flag, value);
                i++;
                }
            }
    }
}

int main(int argc, char **argv) {
    options_t options;

    if (argc > 1) {
        parse_args(argc, argv, &options);
        if (options.policy == FIFO || options.policy == SJF) {
            job_list_random(&options);
            show_jobs_fifo_sjf(&options);
        } // ajouter la condition if options.policy == RR alors on init round robin jobs, et on affiche round robin.
        if (options.use_seed == true) {
        srand(options.seed_value);
        } else {
            srand(time(NULL));
        }
    } else {
        job_list_random(&options);
        show_jobs(&options);
    }

    return 0;
}