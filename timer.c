/* cc timer.c -o timer */

/*
 * usage: timer [options]
 *  
 * options: -s <int seconds>
 *          -m <int minutes>
 *          -h <int hours>
*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define RED "\x1b[0;31m"
#define DEFAULT "\x1b[0;39m"

void sigint_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n"RED"stopping\n");
        exit(0);
    }
}

int isnum(char c) {
    return c >= '0' && c <= '9';
}

static void delay(unsigned int ms) {
    clock_t start_time = clock();
    clock_t wait_time = ms * (CLOCKS_PER_SEC / 1000);

    while (clock() - start_time < wait_time);

}

void print_usage() {
   printf("usage: timer [ -h <int hours> -m <int minutes> -s <int seconds>]\n");
}

typedef struct {
    int val;
    int enabled;
} flag_t;

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);

    if (argc == 1) {
        print_usage();
        return 1;
    }

    /* initialize flags */
    flag_t seconds;
    flag_t minutes;
    flag_t hours;

    seconds.val     = 0;
    seconds.enabled = 0;
    minutes.val     = 0;
    minutes.enabled = 0;
    hours.val       = 0;
    hours.enabled   = 0;


    /* check if every flag has a value */
    if ((argc - 1) % 2 == 1) {
        fprintf(stderr, RED"error:"DEFAULT" no valid flags given :(\n");
        return 1;
    }

    /* parse flags */
    int i;
    for (i = 1; i < argc; i += 2) {
        if (!strcmp("-s", argv[i])) {
            seconds.val     = atoi(argv[i + 1]);
            seconds.enabled = 1;
        } else if (!strcmp("-m", argv[i])) {
            minutes.val     = atoi(argv[i + 1]);
            minutes.enabled = 1;
        } else if (!strcmp("-h", argv[i])) {
            hours.val     = atoi(argv[i + 1]);
            hours.enabled = 1;
        }
    }

    /* check if flags given are valid */
    if (!(seconds.enabled || minutes.enabled || hours.enabled)) {
        fprintf(stderr, RED"error:"DEFAULT" no valid flags given :(\n");
        return 1;
    }

    /* check if flag inputs are valid */
    if (hours.val < 0) {
        fprintf(stderr, RED"error:"DEFAULT" -h flag input is not an int :(\n");
        return 1;
    } if (minutes.val < 0) {
        fprintf(stderr, RED"error:"DEFAULT" -m flag input is not an int :(\n");
        return 1;
    } if (seconds.val < 0) {
        fprintf(stderr, RED"error:"DEFAULT" -s flag input is not an int :(\n");
        return 1;
    }

    int timer;

    timer = seconds.enabled * seconds.val 
          + minutes.enabled * minutes.val * 60
          +   hours.enabled *   hours.val * 3600;

    while (timer > 0) {
        long h = timer / 3600;
        long m = (timer % 3600) / 60;
        long s = timer % 60;

        printf("\r%ld:%02ld:%02ld", h, m, s);
        fflush(stdout);

        timer--;
        delay(1000);
    }

    printf("\r\x1b[K");
    printf("done!\n\a");
    return 0;
}
