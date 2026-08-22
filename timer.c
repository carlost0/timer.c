
/*
 * timer.c: simple terminal timer written in ansi c
 * usage: timer [-cS] [options]
 *  
 * options: -c countdown 
 *          -S stopwatch
 *          -s <int seconds>
 *          -m <int minutes>
 *          -h <int hours>
 *
 * compile with: cc timer.c -o timer
*/ 

/*
 * Copyright (C) 2026 Carlos G. S.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact: Instagram DMs @cgs.dev
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
    printf(
        "timer.c: simple terminal timer written in ansi c\n"
        "usage: timer [-cS] [options]\n\n"
         
        "options: -c countdown\n"
               "\t-S stopwatch\n"
               "\t-s <int seconds>\n"
               "\t-m <int minutes>\n"
               "\t-h <int hours>\n"
    );
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

    int countdown = 0;
    int stopwatch = 0;
    int time_limit = 1;

    seconds.val     = 0;
    seconds.enabled = 0;
    minutes.val     = 0;
    minutes.enabled = 0;
    hours.val       = 0;
    hours.enabled   = 0;


    /* check if every flag has a value */
    /*
    if ((argc - 1) % 2 == 1) {
        fprintf(stderr, RED"error:"DEFAULT" no valid flags given :(\n");
        return 1;
    } */

    /* parse flags */
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp("-s", argv[i])) {
            seconds.val     = atoi(argv[++i]);
            seconds.enabled = 1;
        } else if (!strcmp("-m", argv[i])) {
            minutes.val     = atoi(argv[++i]);
            minutes.enabled = 1;
        } else if (!strcmp("-h", argv[i])) {
            hours.val     = atoi(argv[++i]);
            hours.enabled = 1;
        } else if (!strcmp("-c", argv[i])) {
            countdown = 1;
        } else if (!strcmp("-S", argv[i])) {
            stopwatch = 1;
        }
    }

    /* check if flags given are valid */
    if (!(seconds.enabled || minutes.enabled || hours.enabled)) {
        time_limit = 0;
    }

    if (!(countdown || stopwatch))
        countdown = 1;
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

    int timer = seconds.enabled * seconds.val 
              + minutes.enabled * minutes.val * 60
              + hours.enabled   *   hours.val * 3600;
    int current = 0;

    if (countdown) {
        if (!time_limit) {
            fprintf(stderr, RED"error:"DEFAULT" no time limit given for countdown :(\n");
            return 1;
        }
        while (timer > 0) {
            long h = timer / 3600;
            long m = (timer % 3600) / 60;
            long s = timer % 60;

            printf("\r%ld:%02ld:%02ld", h, m, s);
            fflush(stdout);

            timer--;
            delay(1000);
        }
    } else if (stopwatch) {
        while (1) {
            long h = timer / 3600;
            long m = (timer % 3600) / 60;
            long s = timer % 60;

            long ch = current / 3600;
            long cm = (current % 3600) / 60;
            long cs = current % 60;

            printf("\r%ld:%02ld:%02ld", ch, cm, cs);
            if (time_limit) {
                printf("/%ld:%02ld:%02ld", h, m, s);
                if (current > timer) break;
            }

            fflush(stdout);

            current++;
            delay(1000);
        }
    }

    printf("\r\x1b[K");
    printf("done!\n\a");
    return 0;
}
