#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define RED "\x1b[0;31m"
#define DEFAULT "\x1b[0;39m"

void sigint_handler(int sig) {
    printf("\n"RED"stopping\n");
    exit(0);
}

int isnum(char c) {
    return c >= '0' && c <= '9';
}
static inline int power(int a, int b) {
    int res = a;
    for (int i = 0; i < b; ++i) res *= a;
    return res;
}

static inline void delay(unsigned int ms) {
    clock_t start_time = clock();
    clock_t wait_time = ms * (CLOCKS_PER_SEC / 1000);

    while (clock() - start_time < wait_time);

}

int stoi(const char *s)  {
    size_t len = strlen(s);
    int sum = 0;
    int i = 1; 

    while (*s != '\0') {
        if (!isnum(*s)) return -1;
        sum += (*s++ - '0') * power(10, len - i);
        i++;
    } 
    return sum;
}

void print_usage(int err) {
   printf("usage: %s [ -h <int hours> -m <int minutes> -s <int seconds>]\n", __FILE__);
}

typedef struct {
    int val;
    int enabled;
} flag_t;

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    flag_t seconds = {0};
    flag_t minutes = {0};
    flag_t hours   = {0};

    if (argc == 1) {
        print_usage(1);
        return 1;
    }

    if ((argc - 1) % 2 == 1) {
        fprintf(stderr, RED"error:"DEFAULT" no valid flags given :(\n");
        return 1;
    }

    for (int i = 1; i < argc; i += 2) {
        if (!strcmp("-s", argv[i])) {
            seconds.val = stoi(argv[i + 1]);
            seconds.enabled = 1;
        } else if (!strcmp("-m", argv[i])) {
            minutes.val = stoi(argv[i + 1]);
            minutes.enabled = 1;
        } else if (!strcmp("-h", argv[i])) {
            hours.val = stoi(argv[i + 1]);
            hours.enabled = 1;
        }
    }
    if (!(seconds.enabled || minutes.enabled || hours.enabled)) {
        fprintf(stderr, RED"error:"DEFAULT" no valid flags given :(\n");
        return 1;
    }

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

    int start = time(NULL);
    int now = 0;
    int old = start;
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
