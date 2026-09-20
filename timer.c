
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

#define NGL_NO_MATH
#define NGL_FONTS_IMPLEMENTATION
#define NGL_IMPLEMENTATION
#include "ngl.h"

#define TRED "\x1b[0;31m"
#define TDEFAULT "\x1b[0;39m"

#define FPS 60

static ngl_t ngl;

void sigint_handler(int sig) {
    if (sig == SIGINT) {
        clear_screen();
        ngl_destroy(&ngl);
        printf("\n"TRED"stopping\n""TDEFAULT");
        exit(0);
    }
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
        fprintf(stderr, TRED"error:"TDEFAULT" -h flag input is not an int :(\n");
        return 1;
    } if (minutes.val < 0) {
        fprintf(stderr, TRED"error:"TDEFAULT" -m flag input is not an int :(\n");
        return 1;
    } if (seconds.val < 0) {
        fprintf(stderr, TRED"error:"TDEFAULT" -s flag input is not an int :(\n");
        return 1;
    }

    int timer = seconds.enabled * seconds.val 
              + minutes.enabled * minutes.val * 60
              + hours.enabled   *   hours.val * 3600;
    int current = 0;

    if (!time_limit && !stopwatch) {
        fprintf(stderr, TRED"error:"TDEFAULT" no time limit given for countdown :(\n");
        return 1;
    }

    ngl = ngl_new(0);

    u32 width = ngl.screen.w;
    u32 height = ngl.screen.h;

    clear_screen();
    if (countdown) {
        u32 len = strlen("00:00:00");
        u64 frame = 0; 
        while (timer > 0) {
            get_keyboard_state(&ngl);
            long h = timer / 3600;
            long m = (timer % 3600) / 60;
            long s = timer % 60;

            fill_bg(&ngl, ':', (color_t){0});
            draw_text_fmt(&ngl, (width / 2) - (len * ngl.font.w / 2), (height / 2) - ngl.font.h, 'l', WHITE, "%02ld:%02ld:%02ld", h, m, s);
            draw_screen_borders(&ngl, 0, WHITE);
            print_screen(&ngl);

            if (frame % FPS == 0)
                timer--;
            frame++;
            delay(1000 / FPS);
            if (is_key_down(&ngl, KEY_Q)) break;
        }
    } else if (stopwatch) {
        u32 len = strlen("00:00:00") + 1;
        size_t cap = len + (len + 2) * time_limit;

        u64 frame = 0; 

        u32 x = (width / 2) - (cap + ngl.font.w / 2);
        u32 y = (height / 2) - ngl.font.h;
        while (1) {
            get_keyboard_state(&ngl);
            long h = timer / 3600;
            long m = (timer % 3600) / 60;
            long s = timer % 60;

            long ch = current / 3600;
            long cm = (current % 3600) / 60;
            long cs = current % 60;


            fill_bg(&ngl, ':', BLACK);

            draw_text_fmt(&ngl, x, y, 'l', (color_t) {255,255,255}, "%02ld:%02ld:%02ld", ch, cm, cs);
            if (time_limit) {
                draw_text_fmt(&ngl, x, y + ngl.font.hpad + ngl.font.h + 2, 'l', (color_t) {255,255,255}, "%02ld:%02ld:%02ld", h, m, s);
                draw_rect(&ngl, x - width / 10, y + ngl.font.h + 1, ngl.font.h * len + width / 5, 1, '-', (color_t) {255,255,255});
            }

            draw_screen_borders(&ngl, 0, WHITE);

            print_screen(&ngl);

            if (frame % FPS == 0)
                current++;

            if (current * time_limit > timer * time_limit) break;

            if (is_key_down(&ngl, KEY_Q)) break;
            frame++;
            delay(1000 / FPS);
        }
    }

    ngl_destroy(&ngl);

    printf("\r\x1b[K");
    printf("done!\n\a"TDEFAULT);
    return 0;
}
