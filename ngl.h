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

/* 
 * ngl.h: not a graphics library is a stb-style single-header
 * Graphics Library for the Console on Linux written in C.
 *
 * compilation:
 *  Link with the mathematical Library (-lm) if NGL_NO_MATH isn't set.
 *
 * usage:
 *  Include ngl.h in your file and define the NGL_IMPLEMENTATION Macro in
 *  your main File.
 * 
 * configuration Macros:
 *  #define NGL_MALLOC:
 *   Set the Memory Allocator used in ngl.h.
 *   Default: stdlib malloc.
 *
 *  #define NGL_FREE:
 *   Set the free function used in ngl.h.
 *   Default: stdlib free.
 *
 *  #define NGL_UNSTRIP_PREFIX: 
 *   Prepend "ngl_" to all functions, this is a Workaround as we don't 
 *   have Namespaces in C
 *
 *  #define NGL_NO_FONTS:
 *   Disable the font Module.
 *
 *  #define NGL_NO_INPUT:
 *   Disable the input Module.
 *
 *  #define NGL_NO_MATH:
 *   Disable the math Module.
 *
 *  #define NGL_SERIOUS_ERRORS:
 *   Removes the ":(" / ":)" at the end of Error messages.
 *
 *  #define NGL_MATHDEF:
 *   Set the way Functions should be defined in the math Module.
 *   Default: static inline.
 *
 *  #define NGL_USE_F64
 *   Make the Math module use 64 bit floats instead of 32 bit.
 *
 * Examples: checkout README.md and example directory.
 */
#ifndef _NGL_H
#define _NGL_H

#if !defined(__linux__) && !defined(NGL_NO_INPUT)
# ifdef NGL_SERIOUS_ERRORS
#  error "ngl input Module currently only works on linux."
# else
#  error "ngl input Module currently only works on linux :(."
# endif /* NGL_SERIOUS_ERRORS */
#endif /* __linux__ */

#undef _GNU_SOURCE
#define _GNU_SOURCE
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>

#include <sys/select.h>
#include <sys/ioctl.h>

#include <fcntl.h>
#include <unistd.h>

/* Input Dependencies. */
#ifndef NGL_NO_INPUT

#include <linux/input.h>

#include <stdbool.h>

#include <termios.h>
#include <errno.h>
#include <poll.h>

#endif /* NGL_NO_INPUT */

/* Font Dependencies. */
#ifndef NGL_NO_FONTS

#include <stdarg.h>

#endif /* NGL_NO_FONTS */

/* Math Dependencies. */
#ifndef NGL_NO_MATH

#include <math.h>

#endif /* NGL_NO_MATH */

/* ngl.h Version. */
#define NGL_VERSION_MAJOR 1
#define NGL_VERSION_MINOR 0
#define NGL_VERSION_PATCH 0

#define NGL_IS_RELEASE 0

#define NGL_VERSION_STR "1.0.2-dev"

/* More helpful types */
typedef int8_t    i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef double   f64;
typedef float    f32;

#ifndef NGL_MALLOC
#define NGL_MALLOC(s) malloc(s)
#endif /* NGL_MALLOC */

#ifndef NGL_FREE
#define NGL_FREE(p) free(p)
#endif /* NGL_FREE */


/*
 * +-------------------------------------------------------------------------+
 * |                            ngl Types/Structs.                           |
 * +-------------------------------------------------------------------------+
 */

/* Basic RGB Color Struct. */
typedef struct { u8 r, g, b; } ngl_color_t;

/* Constructor */
#ifndef ngl_color
#define ngl_color(r,g,b) ((ngl_color_t){r,g,b})
#endif /* ngl_color */

/* Most ngl Functions return this Type. */
/*
 * ngl gives you the Freedom to write Error Messages yourself,
 * or just use the error_to_string function.
 */
typedef enum {
    ERR_SUCCESS = 0,

    ERR_INVALID_SIZE,
    ERR_INVALID_PTR,

    ERR_FAILED_MALLOC,
    ERR_FAILED_FILE_OPEN,
    ERR_FAILED_FILE_READ,
    ERR_FAILED_FILE_CLOSE,
    ERR_FAILED_POLL,

    _ERR_COUNT,
} ngl_error_t;

/* These are two Heap allocated 1d arrays where we will store the "Pixels" to. */
typedef struct {
    ngl_color_t *colors;
    char        *chars;
} ngl_buf_t ;

/* All the drawing will be done to the "next" Buffer this Struct. */
typedef struct {
    u32 w, h;
    ngl_buf_t current;
    ngl_buf_t next;
    ngl_error_t status;
} ngl_screen_t;


/*
 * +-------------------------------------------------------------------------+
 * |                       ngl input Types/Structs.                          |
 * +-------------------------------------------------------------------------+
 */
#ifndef NGL_NO_INPUT

typedef enum {
    KSTATE_UP       = 0,
    KSTATE_DOWN     = 1,
    KSTATE_REPEAT   = 2,
    KSTATE_RELEASED = 3,
} ngl_key_state_t;

typedef struct {
    struct pollfd   pfd;
    struct termios  oldt;
    ngl_key_state_t key_states[KEY_MAX + 1];
    ngl_key_state_t old_key_states[KEY_MAX + 1];
    ngl_error_t     status;
} ngl_input_ctx_t;

#endif /* NGL_NO_INPUT */

/*
 * +-------------------------------------------------------------------------+
 * |                        ngl font Types/Structs.                          |
 * +-------------------------------------------------------------------------+
 */

#ifndef NGL_NO_FONTS

typedef struct {
    u8 w, h;
    u8  hpad, vpad;
    const u32 *glyphs;
} ngl_font_t;

#endif /* NGL_NO_FONTS */

/*
 * All these fields have default Initializers, so they may be left empty when 
 * calling ngl_new.
 */
typedef struct {
#ifndef NGL_NO_FONTS
    struct {
        u32 w, h;
        u8 hpad, vpad;
        const u32 *glyphs;
    } font;
#endif /* NGL_NO_FONTS */

    struct {
        u32 w, h;
    } screen;
} ngl_api_config_t;

typedef struct {
    ngl_screen_t    screen;

#ifndef NGL_NO_INPUT
    ngl_input_ctx_t input;
#endif /* NGL_NO_INPUT */

#ifndef NGL_NO_FONTS
    ngl_font_t      font;
#endif /* NGL_NO_FONTS */

} ngl_t;


/*
 * +-------------------------------------------------------------------------+
 * |                            ngl main Module.                             |
 * +-------------------------------------------------------------------------+
 */

/* Get an Error string out of an Error Code. */
const char *ngl_error_to_string(ngl_error_t error);

/* Default colors. */
#ifndef NGL_BLACK
#define NGL_BLACK ngl_color(0,0,0)
#endif /* NGL_BLACK */

#ifndef NGL_WHITE
#define NGL_WHITE ngl_color(255,255,255)
#endif /* NGL_WHITE */

#ifndef NGL_RED
#define NGL_RED ngl_color(255,0,0)
#endif /* NGL_RED */

#ifndef NGL_GREEN
#define NGL_GREEN ngl_color(0,255,0)
#endif /* NGL_GREEN */

#ifndef NGL_YELLOW
#define NGL_YELLOW ngl_color(255,255,0)
#endif /* NGL_YELLOW */

#ifndef NGL_BLUE
#define NGL_BLUE ngl_color(0,0,255)
#endif /* NGL_BLUE */

#ifndef NGL_MAGENTA
#define NGL_MAGENTA ngl_color(255,0,255)
#endif /* NGL_MAGENTA */

#ifndef NGL_CYAN
#define NGL_CYAN ngl_color(0,255,255)
#endif /* NGL_CYAN */


/* Convert a 2d Coordinate into a 1d Index. */

#ifndef ngl_idx
#define ngl_idx(x, y, w) ((y) * (w) + (x))
#endif /* ngl_idx */

/* Create a new ngl API context, only works with designated initializers.
 * 
 * example:
 *  ngl_t ngl = ngl_new(.screen.w = 32, .screen.h = 32);
 */

ngl_t _ngl_new(ngl_api_config_t config);
#define ngl_new(...) _ngl_new((ngl_api_config_t){__VA_ARGS__})
void ngl_destroy(ngl_t *api);

void ngl_delay(u32 ms);
u64  ngl_get_ms(void);
void ngl_clear_screen(void);

ngl_error_t  ngl_get_term_size(u32 *w, u32 *h);

ngl_error_t  ngl_init_screen(ngl_screen_t *screen);
ngl_screen_t ngl_screen_new(u32 w, u32 h);
ngl_error_t  ngl_destroy_screen(ngl_screen_t *screen);

ngl_error_t  ngl_print_screen(ngl_t *api);



ngl_error_t  ngl_fill_bg(ngl_t *api, char c, ngl_color_t color);

ngl_error_t  ngl_set_pixel(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color);

ngl_error_t  ngl_draw_screen_borders(ngl_t *api, char c, ngl_color_t color); 

ngl_error_t  ngl_draw_rect(ngl_t *api, u32 x, u32 y, u32 w, u32 h, char c, ngl_color_t color);

/* Draw a string with w*h dimensions. */
/* NOTE: The String must be at least w*h Bytes long, else there would be a Buffer overflow. */ 
ngl_error_t  ngl_draw_sprite(ngl_t *api, u32 x, u32 y, u32 w, u32 h, const char *sprite, ngl_color_t color);

ngl_error_t  ngl_draw_line(ngl_t *api, u32 start_x, u32 start_y, u32 end_x, u32 end_y, char c, ngl_color_t color);




#ifndef NGL_NO_INPUT

/*
 * +-------------------------------------------------------------------------+
 * |                            ngl input Module.                            |
 * +-------------------------------------------------------------------------+
 */


ngl_input_ctx_t ngl_input_new(void);
ngl_error_t     ngl_init_input(ngl_input_ctx_t *ctx);
ngl_error_t     ngl_destroy_input(ngl_input_ctx_t *ctx);

/* This function should only be called once per Frame. */
ngl_key_state_t ngl_get_key_state(ngl_t *api, u16 key);

ngl_error_t     ngl_get_keyboard_state(ngl_t *api);


#ifndef ngl_is_key_down
#define ngl_is_key_down(api, key)           (ngl_get_key_state(api, key) > 0 && ngl_get_key_state(api, key) < 3)
#endif /* is_key_down */


#ifndef  ngl_is_key_pressed_repeat
#define ngl_is_key_pressed_repeat(api, key) (ngl_get_key_state(api, key) == KSTATE_REPEAT)
#endif /* is_key_pressed_repeat */

#ifndef ngl_is_key_released
#define ngl_is_key_released(api, key)       (ngl_get_key_state(api, key) == KSTATE_RELEASED)
#endif /* is_key_down */


#endif /* NGL_NO_INPUT */




#ifndef NGL_NO_FONTS

/*
 * +-------------------------------------------------------------------------+
 * |                            ngl font Module.                             |
 * +-------------------------------------------------------------------------+
 */



/* 
 * ngl fonts are made with nglfontbuilder.c, found in extra/.
 * Since implementing ttf support would be overkill (I'm to lazy),
 * we must make some limitations:
 *  - The Glyphs must not be bigger than 5x5 pixels.
 *  - The Glyphs must only be symbols found in the ASCII table
 *    starting from '!' until '~'.
 *  - The Glyphs must be ordered the same way as they appear
 *    in the ASCII table.
 */

/* Load specified glyphs into font, if the second parameter is NULL, the default glyphs will be used. */
ngl_error_t ngl_load_glyphs(ngl_font_t *font, const u32 *glyphs);

ngl_error_t ngl_draw_glyph(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color, char glyph);
ngl_error_t ngl_draw_text(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color, const char *str);
ngl_error_t ngl_draw_text_fmt(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color, const char *format, ...);

#endif /* NGL_NO_FONTS */




#ifndef NGL_NO_MATH

/*
 * +-------------------------------------------------------------------------+
 * |                            ngl math Module.                             |
 * +-------------------------------------------------------------------------+
 */



#ifndef NGL_MATHDEF
#define NGL_MATHDEF static inline 
#endif /* NGL_MATHDEF */


#ifndef NGL_USE_F64
#define ngl_float f32
#define NGL_SQRT sqrtf
#define NGL_SIN sinf
#define NGL_COS cosf
#endif /* ngl_float */

typedef struct {
    ngl_float x, y;
} ngl_vec2_t;

/* Constructor */
#ifndef ngl_vec2
#define ngl_vec2(x, y) ((ngl_vec2_t){(ngl_float)(x), (ngl_float)(y)})
#endif /* ngl_vec2 */

#ifndef PI
#define PI 3.14159265358979323846f
#endif /* PI */

#ifndef ngl_deg_to_rad
#define ngl_deg_to_rad(theta) ((theta) * (PI / 180.0f))
#endif /* ngl_deg_to_rad */

NGL_MATHDEF ngl_vec2_t ngl_vec2_add(ngl_vec2_t a, ngl_vec2_t b);
NGL_MATHDEF ngl_vec2_t ngl_vec2_sub(ngl_vec2_t a, ngl_vec2_t b);
NGL_MATHDEF ngl_vec2_t ngl_vec2_mul(ngl_vec2_t a, ngl_vec2_t b);
NGL_MATHDEF ngl_vec2_t ngl_vec2_div(ngl_vec2_t a, ngl_vec2_t b);
NGL_MATHDEF ngl_vec2_t ngl_vec2_sqrt(ngl_vec2_t vec);
NGL_MATHDEF ngl_vec2_t ngl_vec2_scale(ngl_vec2_t vec, ngl_float scalar);

NGL_MATHDEF ngl_float  ngl_vec2_dot(ngl_vec2_t a, ngl_vec2_t b);
NGL_MATHDEF ngl_float  ngl_vec2_cross(ngl_vec2_t a, ngl_vec2_t b);

NGL_MATHDEF ngl_float  ngl_vec2_len(ngl_vec2_t vec);
NGL_MATHDEF ngl_vec2_t ngl_vec2_normalize(ngl_vec2_t vec);
NGL_MATHDEF ngl_vec2_t ngl_vec2_rot90cw(ngl_vec2_t vec);
NGL_MATHDEF ngl_vec2_t ngl_vec2_rot90ccw(ngl_vec2_t vec);

/* Rotate Vector by angle in radians */
NGL_MATHDEF ngl_vec2_t ngl_vec2_rot(ngl_vec2_t vec, ngl_float angle);

#ifndef ngl_vec2_mag
#define ngl_vec2_mag ngl_vec2_len
#endif /* ngl_vec2_mag */



#endif /* NGL_NO_MATH */

#endif /* _NGL_H */


#ifdef NGL_IMPLEMENTATION

/*
 * +-------------------------------------------------------------------------+
 * |                            ngl main Implementation.                     |
 * +-------------------------------------------------------------------------+
 */

ngl_t _ngl_new(ngl_api_config_t config) {
    ngl_t api = {0};

    u32 screen_w = 0, screen_h = 0;
    if (!(config.screen.w && config.screen.h)) {
        ngl_get_term_size(&screen_w, &screen_h);
        if (screen_h) screen_h--;
    } else {
        screen_w = config.screen.w;
        screen_h = config.screen.h;
    }

    api.screen = ngl_screen_new(screen_w, screen_h);

#ifndef NGL_NO_INPUT
    api.input = ngl_input_new();
#endif /* NGL_NO_INPUT */
    
#ifndef NGL_NO_FONTS
    api.font = (ngl_font_t) {
        .w = config.font.w,
        .h = config.font.h,
        .vpad = config.font.vpad,
        .hpad = config.font.hpad,
    };

    ngl_load_glyphs(&api.font, config.font.glyphs);
#endif /* NGL_NO_FONTS */

    return api;
}

void ngl_destroy(ngl_t *api) {
#ifndef NGL_NO_INPUT
    ngl_destroy_input(&api->input);
#endif /* NGL_NO_INPUT */
    ngl_destroy_screen(&api->screen);
}

ngl_error_t ngl_init_screen(ngl_screen_t *screen) {
    if (!screen) return ERR_INVALID_PTR;
    if (screen->w <= 1 || screen->h <= 1) return ERR_INVALID_SIZE;
    if (screen->w > 10000 || screen->h > 10000) return ERR_INVALID_SIZE;

    u32 n = screen->w * screen->h;

    /* Allocate front Buffer. */
    screen->current.colors = (ngl_color_t*)NGL_MALLOC(n * sizeof(ngl_color_t));
    if (!screen->current.colors) return ERR_FAILED_MALLOC;

    screen->current.chars = (char*)NGL_MALLOC(n * sizeof(char));
    if (!screen->current.chars) {
        NGL_FREE(screen->current.colors);
        return ERR_FAILED_MALLOC;
    }


    /* Allocate back Buffer. */
    screen->next.colors = (ngl_color_t*)NGL_MALLOC(n * sizeof(ngl_color_t));
    if (!screen->next.colors) {
        NGL_FREE(screen->current.colors);
        NGL_FREE(screen->current.chars);
        return ERR_FAILED_MALLOC;
    }

    screen->next.chars = (char*)NGL_MALLOC(n * sizeof(char));
    if (!screen->next.chars) {
        NGL_FREE(screen->next.colors);
        NGL_FREE(screen->current.colors);
        NGL_FREE(screen->current.chars);
        return ERR_FAILED_MALLOC;
    }

    /* Zero-initialize both Buffers. */
    memset(screen->current.colors, 0, n * sizeof(ngl_color_t));
    memset(screen->current.chars,  ' ', n * sizeof(char));

    memset(screen->next.colors, 0, n * sizeof(ngl_color_t));
    memset(screen->next.chars,  ' ', n * sizeof(char));

    return ERR_SUCCESS;
}

ngl_screen_t ngl_screen_new(u32 w, u32 h) {
    ngl_screen_t screen = {w, h, {0}, {0}, ERR_SUCCESS};
    ngl_error_t err = ngl_init_screen(&screen);
    screen.status = err;

    return screen;
}

#ifdef NGL_SERIOUS_ERRORS
const char *ngl_error_to_string(ngl_error_t error) {
    if (error >= _ERR_COUNT) return "Unknown Error";
    switch (error) {
        case ERR_SUCCESS:
            return "No error.";
        case ERR_FAILED_POLL:
            return "Failed poll.";
        case ERR_INVALID_PTR:
            return "Invalid pointer.";
        case ERR_INVALID_SIZE:
            return "Invalid size.";
        case ERR_FAILED_MALLOC:
            return "Failed malloc.";
        case ERR_FAILED_FILE_READ:
            return "Failed to read file.";
        case ERR_FAILED_FILE_OPEN:
            return "Failed to open file.";
        case ERR_FAILED_FILE_CLOSE:
            return "Failed to close file.";
        default:
            return "Unknown error";
    }
}

#else 
const char *ngl_error_to_string(ngl_error_t error) {
    if (error >= _ERR_COUNT) return "Unknown Error";
    switch (error) {
        case ERR_SUCCESS:
            return "No error! :)";
        case ERR_FAILED_POLL:
            return "Failed poll. :(";
        case ERR_INVALID_PTR:
            return "Invalid pointer. :(";
        case ERR_INVALID_SIZE:
            return "Invalid size. :(";
        case ERR_FAILED_MALLOC:
            return "Failed malloc. (buy more ram lol)";
        case ERR_FAILED_FILE_READ:
            return "Failed to read file. :(";
        case ERR_FAILED_FILE_OPEN:
            return "Failed to open file. :(";
        case ERR_FAILED_FILE_CLOSE:
            return "Failed to close file. :(";
        default:
            return "Unknown error :|";
    }
}
#endif /* NGL_SERIOUS_ERRORS */

void ngl_delay(u32 ms) {
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000L;
    select(0, NULL, NULL, NULL, &tv);
}

u64 ngl_get_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (u64)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* Function originaly written by Glenn Chappell & Ian Chai 14 Apr 1993 */
ngl_error_t ngl_get_term_size(u32 *w, u32 *h) {
    if (!(w && h)) return ERR_INVALID_PTR;
    struct winsize ws;
    int fd,result;

    if ((fd = open("/dev/tty",O_RDONLY))<0) return ERR_FAILED_FILE_OPEN;

    result = ioctl(fd,TIOCGWINSZ,&ws);
    close(fd);

    if (result < 0) return ERR_FAILED_FILE_OPEN;

    *w = (u32)ws.ws_col;
    *h = (u32)ws.ws_row;
    return ERR_SUCCESS;
}

ngl_error_t ngl_destroy_screen(ngl_screen_t *screen) {
    if (!screen) return ERR_INVALID_PTR;

    ngl_error_t err = ERR_SUCCESS;
    /* Free front Buffer. */
    if (screen->current.colors) NGL_FREE(screen->current.colors);
    if (screen->current.chars) NGL_FREE(screen->current.chars);

    /* Free back Buffer. */
    if (screen->next.colors) NGL_FREE(screen->next.colors);
    if (screen->next.chars) NGL_FREE(screen->next.chars);
     
    *screen = (ngl_screen_t){0};

    return err;
}


ngl_error_t ngl_fill_bg(ngl_t *api, char c, ngl_color_t color) {
    ngl_screen_t *screen = &api->screen;

    if (!screen || !(screen->next.chars && screen->next.colors)) return ERR_INVALID_PTR;

    /* Set the whole screen to the specified Chars and Colors. */
    u32 i;
    for (i = 0; i < screen->h * screen->w; ++i) {
        screen->next.colors[i] = color;
        screen->next.chars[i] = c;
    }

    return ERR_SUCCESS;
}

void ngl_clear_screen(void) {
    /* ANSI Code to return the Cursor to the Home row and clear everything after the Cursor */
    printf("\x1b[H\x1b[J");
    fflush(stdout);
}

ngl_error_t ngl_print_screen(ngl_t *api) {
    ngl_screen_t *screen = &api->screen;
    if (!screen || !(screen->next.chars && screen->next.colors)) return ERR_INVALID_PTR;

    /* The length of "\x1b[38;2;255;255;255mC". */
    u32 worst_case_pixel_size = 23;
    u32 cap = worst_case_pixel_size * screen->w * screen->h;

    /*
     * We draw into a temporary Buffer and write it all
     * at once to stdout to reduce CPU Usage and Flickering.
     */
    char *buf = (char *)NGL_MALLOC(cap * sizeof(char));
    if (!buf) return ERR_FAILED_MALLOC;
    u32 pos = 0;

    ngl_color_t last_col = {0,0,0};

    pos += snprintf(buf+pos, cap-pos, "\x1b[H");

    u32 x, y;
    for (y = 0; y < screen->h; ++y) {
        for (x = 0; x < screen->w; ++x) {
            if (pos + 24 >= cap) {
                NGL_FREE(buf);
                return ERR_INVALID_SIZE;
            }

            /* current index, character and color */
            u32 i = ngl_idx(x, y, screen->w);
            char cchar          = screen->current.chars[i];
            ngl_color_t ccol = screen->current.colors[i];

            /*
             * Since ANSI Escape Codes don't reset the color automatically,
             * we only need to change the Color if it's different than the
             * previous color.
             */
            if (!(ccol.r == last_col.r && ccol.g == last_col.g && ccol.b == last_col.b)) {
                pos += snprintf(buf+pos, cap-pos, "\x1b[38;2;%u;%u;%um", ccol.r, ccol.g, ccol.b);
                last_col = ccol;
            }

            buf[pos++] = cchar;
        }
        buf[pos++] = '\n';
    }

    fwrite(buf, sizeof(char), pos, stdout);
    fflush(stdout);
    NGL_FREE(buf);

    /* Swap buffers. */
    ngl_buf_t tmp = screen->current;
    screen->current = screen->next;
    screen->next = tmp;

    return ERR_SUCCESS;
}

ngl_error_t ngl_draw_screen_borders(ngl_t *api, char c, ngl_color_t color) {
    ngl_screen_t *screen = &api->screen;

    if (!screen || !(screen->next.chars && screen->next.colors)) return ERR_INVALID_PTR;
    if (screen->w <= 1 || screen->h <= 1) return ERR_INVALID_SIZE;

    u32 w = screen->w;
    u32 h = screen->h;

    char hchar = c ? c : '-';
    char vchar = c ? c : '|';
    char corner = c ? c : '+';

    /* Draw top and bottom lines at the same Time */
    u32 x;
    for (x = 0; x < w; ++x) {
        u32 i = ngl_idx(x, 0, w);
        u32 j = ngl_idx(x, h-1, w);

        screen->next.chars[i] = hchar;
        screen->next.chars[j] = hchar;
        screen->next.colors[i] = color;
        screen->next.colors[j] = color;
    }

    /* Draw right and left lines at the same Time */
    u32 y;
    for (y = 1; y < h; ++y) {
        u32 i = ngl_idx(0, y, w);
        u32 j = ngl_idx(w-1, y, w);

        screen->next.chars[i] = vchar;
        screen->next.chars[j] = vchar;

        screen->next.colors[i] = color;
        screen->next.colors[j] = color;
    }

    /* Place the Corners. */

    /* Top left Corner. */
    screen->next.chars[0]  = corner;
    screen->next.colors[0] = color;

    /* Top right Corner. */
    screen->next.chars[w-1]  = corner;
    screen->next.colors[w-1] = color;

    /* Bottom left Corner. */
    screen->next.chars[ngl_idx(0, h-1, w)] = corner;
    screen->next.colors[ngl_idx(0, h-1, w)] = color;

    /* Bottom right Corner. */
    screen->next.chars[ngl_idx(w-1, h-1, w)] = corner;
    screen->next.colors[ngl_idx(w-1, h-1, w)] = color;
    return ERR_SUCCESS;
}

ngl_error_t ngl_set_pixel(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color) {
    ngl_screen_t *screen = &api->screen;

    if (!screen || !(screen->next.colors && screen->next.chars)) return ERR_INVALID_PTR;
    if (x >= screen->w || y >= screen->h) return ERR_INVALID_SIZE;

    screen->next.chars[ngl_idx(x, y, screen->w)] = c;
    screen->next.colors[ngl_idx(x, y, screen->w)] = color;

    return ERR_SUCCESS;
}

ngl_error_t ngl_draw_rect(ngl_t *api, u32 x, u32 y, u32 w, u32 h, char c, ngl_color_t color) {
    ngl_screen_t *screen = &api->screen;

    if (!screen || !(screen->next.colors && screen->next.chars)) return ERR_INVALID_PTR;
    if (x >= screen->w || w > screen->w - x || y >= screen->h || h > screen->h - y) return ERR_INVALID_SIZE;

    u32 cx, cy;
    for (cy = y; cy < y + h; ++cy) {
        u32 i = ngl_idx(x, cy, screen->w);
        memset(&screen->next.chars[i], c, w);
        for (cx = 0; cx < w; ++cx) screen->next.colors[i+cx] = color;
    }

    return ERR_SUCCESS;
}


/* NOTE: The String must be at least w*h Bytes long, else there would be a Buffer overflow. */ 
ngl_error_t ngl_draw_sprite(ngl_t *api, u32 x, u32 y, u32 w, u32 h, const char *sprite, ngl_color_t color) {
    ngl_screen_t *screen = &api->screen;

    if (!screen || !(screen->next.chars && screen->next.colors)) return ERR_INVALID_PTR;
    if (x >= screen->w || w > screen->w - x || y >= screen->h || h > screen->h - y) return ERR_INVALID_SIZE;

    u32 cx, cy;
    for (cy = y; cy < y + h; ++cy) {
        for (cx = x; cx < x + w; ++cx) {
            u32 buf_i = ngl_idx(cx, cy, screen->w);
            u32 sprite_i = ngl_idx(cx-x, cy-y, w);
            if (sprite[sprite_i] != ' ') {
                screen->next.chars[buf_i] = sprite[sprite_i];
                screen->next.colors[buf_i] = color;
            }
        }
    }

    return ERR_SUCCESS;
}

ngl_error_t ngl_draw_line(ngl_t *api, u32 start_x, u32 start_y, u32 end_x, u32 end_y, char c, ngl_color_t color) {
    ngl_screen_t *screen = &api->screen;

    if (!screen || !screen->next.colors || !screen->next.chars) return ERR_INVALID_PTR;
    if (start_x >= screen->w || start_y >= screen->h || end_x >= screen->w || end_y >= screen->h) return ERR_INVALID_SIZE;

    i32 x0 = (i32)start_x;
    i32 y0 = (i32)start_y;
    i32 x1 = (i32)end_x;
    i32 y1 = (i32)end_y;

    i32 dx = abs(x1 - x0);
    i32 dy = abs(y1 - y0);

    i32 sx = x0 < x1 ? 1 : -1;
    i32 sy = y0 < y1 ? 1 : -1;

    i32 err = dx - dy;

    while (1) {
        u32 i = ngl_idx((u32)x0, (u32)y0, screen->w);
        screen->next.chars[i] = c;
        screen->next.colors[i] = color;

        if (x0 == x1 && y0 == y1)
            break;

        i32 twice_err = 2 * err;

        if (twice_err > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (twice_err < dx) {
            err += dx;
            y0 += sy;
        }
    }

    return ERR_SUCCESS;
}



#ifndef NGL_NO_INPUT

/*
 * +-------------------------------------------------------------------------+
 * |                            ngl input Implementation.                    |
 * +-------------------------------------------------------------------------+
 */

ngl_key_state_t ngl_get_key_state(ngl_t *api, u16 key) {
    if (key > KEY_MAX) return KSTATE_UP;

    if (api->input.old_key_states[key] != KSTATE_UP && api->input.key_states[key] == KSTATE_UP) return KSTATE_RELEASED;
    return api->input.key_states[key];
}

static inline bool _ngl_test_bit(const u64 *bits, i32 bit) {
    return bits[bit / (sizeof(u64) * 8)] &
           (1ULL << (bit % (sizeof(u64) * 8)));
}

static bool _ngl_is_keyboard(i32 fd) {
    u64 ev_bits[(EV_MAX + 1 +
                            sizeof(unsigned long) * 8 - 1) /
                           (sizeof(unsigned long) * 8)] = {0};

    u64 key_bits[(KEY_MAX + 1 +
                             sizeof(unsigned long) * 8 - 1) /
                            (sizeof(unsigned long) * 8)] = {0};

    if (ioctl(fd, (int)EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0)
        return false;

    if (!_ngl_test_bit(ev_bits, EV_KEY))
        return false;

    if (ioctl(fd, (int)EVIOCGBIT(EV_KEY, sizeof(key_bits)), key_bits) < 0)
        return false;
    return _ngl_test_bit(key_bits, KEY_A) &&
       _ngl_test_bit(key_bits, KEY_Z) &&
       _ngl_test_bit(key_bits, KEY_ENTER) &&
       _ngl_test_bit(key_bits, KEY_SPACE);
}

static i32 _ngl_find_keyboard(void) {
    char path[64];
    u32 i;
    for (i = 0; i < 32; ++i) {
        snprintf(path, sizeof(path), "/dev/input/event%d", i);

        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0)
            continue;

        if (_ngl_is_keyboard(fd))
            return fd;

        close(fd);
    }

    return -1;
}

ngl_error_t ngl_init_input(ngl_input_ctx_t *ctx) {
    int fd = _ngl_find_keyboard();
    if (fd == -1) {
        return ERR_FAILED_FILE_OPEN;
    }

    ctx->pfd.fd = fd;
    ctx->pfd.events = POLLIN;

    memset(&ctx->key_states, 0, sizeof(ngl_key_state_t) * (KEY_MAX + 1));


    /* Get the current terminal Settings. */
    tcgetattr(STDIN_FILENO, &ctx->oldt);
    struct termios newt = ctx->oldt;

    /* Disable echo .*/
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    return ERR_SUCCESS;
}

ngl_input_ctx_t ngl_input_new(void) {
    ngl_input_ctx_t ctx = {0};
    ngl_error_t err = ngl_init_input(&ctx);
    ctx.status = err;
    return ctx;
}

ngl_error_t ngl_destroy_input(ngl_input_ctx_t *ctx) {
    if (!ctx) return ERR_INVALID_PTR;

    tcsetattr(STDIN_FILENO, TCSANOW, &ctx->oldt);

    if (close(ctx->pfd.fd) < 0) return ERR_FAILED_FILE_CLOSE;

    return ERR_SUCCESS;
}

/* This Function is intended to only be called once per Frame. */
ngl_error_t ngl_get_keyboard_state(ngl_t *api) {
    ngl_input_ctx_t *ctx = &api->input;
    struct pollfd *fd = &ctx->pfd;

    fd->revents = 0;
    i32 status = poll(fd, 1, 0);

    if (status < 0 && errno == EINTR) return ERR_SUCCESS;
    
    if (fd->revents & (POLLERR|POLLHUP|POLLNVAL)) {
        return ERR_FAILED_POLL;
    }

    memcpy(&ctx->old_key_states, &ctx->key_states, sizeof(ctx->key_states));

    if (fd->revents & POLLIN) {
        struct input_event events[64];

        ssize_t bytes = read(fd->fd, events, sizeof(events));

        if (bytes == -1) {
            return ERR_FAILED_FILE_READ;
        }

        if (bytes % sizeof(struct input_event) != 0) {
            return ERR_FAILED_FILE_READ;
        }

        size_t count = bytes / sizeof(struct input_event);

        u32 i;
        for (i = 0; i < count; i++) {
            struct input_event *ev = &events[i];

            if (ev->type == EV_KEY) {
                ctx->key_states[ev->code] = (ngl_key_state_t)ev->value;
            }
        }
    }
    return ERR_SUCCESS;
}


#endif /* NGL_NO_INPUT*/
/*
 * +-------------------------------------------------------------------------+
 * |                            ngl font Implementation.                     |
 * +-------------------------------------------------------------------------+
 */


#ifndef NGL_NO_FONTS

#ifndef _NGL_GLYPH
#define _NGL_GLYPH(a, b, c, d, e) \
    ((u32)(a)        |       \
    ((u32)(b) << 5)  |       \
    ((u32)(c) << 10) |       \
    ((u32)(d) << 15) |       \
    ((u32)(e) << 20))
#endif /* _NGL_GLYPH */


#define _NGL_DEFAULT_GLYPH_W (5)
#define _NGL_DEFAULT_GLYPH_H (5)
#define _NGL_DEFAULT_GLYPH_VPAD (1)
#define _NGL_DEFAULT_GLYPH_HPAD (1)

/* 
 * This font is made with nglfontbuilder.c, found in extra/.
 * Since implementing ttf support would be overkill, we must make some limitations:
 *  - The Glyphs must not be bigger than 5x5 pixels.
 *  - The Glyphs must only be symbols found in the ASCII table starting from '!' until '~'.
 *  - The Glyphs must be ordered the same way as they appear in the ASCII table.
 */

static const u32 _NGL_DEFAULT_GLYPHS[] = {
    /* ! */ _NGL_GLYPH(0x01,0x01,0x01,0x00,0x01),
    /* " */ _NGL_GLYPH(0x05,0x05,0x00,0x00,0x00),
    /* # */ _NGL_GLYPH(0x0A,0x1F,0x0A,0x1F,0x0A),
    /* $ */ _NGL_GLYPH(0x0E,0x05,0x0E,0x14,0x0E),
    /* % */ _NGL_GLYPH(0x11,0x08,0x04,0x02,0x11),
    /* & */ _NGL_GLYPH(0x06,0x05,0x16,0x09,0x16),
    /* ' */ _NGL_GLYPH(0x01,0x01,0x00,0x00,0x00),
    /* ( */ _NGL_GLYPH(0x02,0x01,0x01,0x01,0x02),
    /* ) */ _NGL_GLYPH(0x01,0x02,0x02,0x02,0x01),
    /* * */ _NGL_GLYPH(0x04,0x0E,0x04,0x00,0x00),
    /* + */ _NGL_GLYPH(0x00,0x04,0x0E,0x04,0x00),
    /* , */ _NGL_GLYPH(0x00,0x00,0x00,0x00,0x03),
    /* - */ _NGL_GLYPH(0x00,0x00,0x0E,0x00,0x00),
    /* . */ _NGL_GLYPH(0x00,0x00,0x00,0x00,0x01),
    /* / */ _NGL_GLYPH(0x10,0x08,0x04,0x02,0x01),

    /* 0 */ _NGL_GLYPH(0x07,0x05,0x05,0x05,0x07),
    /* 1 */ _NGL_GLYPH(0x04,0x06,0x04,0x04,0x04),
    /* 2 */ _NGL_GLYPH(0x07,0x04,0x07,0x01,0x07),
    /* 3 */ _NGL_GLYPH(0x07,0x04,0x06,0x04,0x07),
    /* 4 */ _NGL_GLYPH(0x05,0x05,0x07,0x04,0x04),
    /* 5 */ _NGL_GLYPH(0x07,0x01,0x07,0x04,0x07),
    /* 6 */ _NGL_GLYPH(0x07,0x01,0x07,0x05,0x07),
    /* 7 */ _NGL_GLYPH(0x07,0x05,0x04,0x04,0x04),
    /* 8 */ _NGL_GLYPH(0x07,0x05,0x07,0x05,0x07),
    /* 9 */ _NGL_GLYPH(0x07,0x05,0x07,0x04,0x07),

    /* : */ _NGL_GLYPH(0x00,0x00,0x01,0x00,0x01),
    /* ; */ _NGL_GLYPH(0x00,0x00,0x02,0x00,0x03),
    /* < */ _NGL_GLYPH(0x00,0x0C,0x02,0x0C,0x00),
    /* = */ _NGL_GLYPH(0x00,0x0E,0x00,0x0E,0x00),
    /* > */ _NGL_GLYPH(0x00,0x06,0x08,0x06,0x00),
    /* ? */ _NGL_GLYPH(0x0E,0x08,0x0C,0x00,0x04),
    /* @ */ _NGL_GLYPH(0x1F,0x11,0x1D,0x01,0x1F),

    /* A */ _NGL_GLYPH(0x0E,0x11,0x11,0x1F,0x11),
    /* B */ _NGL_GLYPH(0x0F,0x11,0x0F,0x11,0x0F),
    /* C */ _NGL_GLYPH(0x1F,0x01,0x01,0x01,0x1F),
    /* D */ _NGL_GLYPH(0x0F,0x11,0x11,0x11,0x0F),
    /* E */ _NGL_GLYPH(0x1F,0x01,0x0F,0x01,0x1F),
    /* F */ _NGL_GLYPH(0x1F,0x01,0x0F,0x01,0x01),
    /* G */ _NGL_GLYPH(0x1F,0x01,0x1D,0x11,0x1F),
    /* H */ _NGL_GLYPH(0x11,0x11,0x1F,0x11,0x11),
    /* I */ _NGL_GLYPH(0x07,0x02,0x02,0x02,0x07),
    /* J */ _NGL_GLYPH(0x1F,0x10,0x10,0x10,0x0F),
    /* K */ _NGL_GLYPH(0x11,0x09,0x07,0x09,0x11),
    /* L */ _NGL_GLYPH(0x01,0x01,0x01,0x01,0x1F),
    /* M */ _NGL_GLYPH(0x0A,0x15,0x15,0x15,0x11),
    /* N */ _NGL_GLYPH(0x11,0x13,0x15,0x19,0x11),
    /* O */ _NGL_GLYPH(0x1F,0x11,0x11,0x11,0x1F),
    /* P */ _NGL_GLYPH(0x1F,0x11,0x1F,0x01,0x01),
    /* Q */ _NGL_GLYPH(0x1F,0x11,0x11,0x09,0x17),
    /* R */ _NGL_GLYPH(0x1F,0x11,0x1F,0x09,0x11),
    /* S */ _NGL_GLYPH(0x1F,0x01,0x1F,0x10,0x1F),
    /* T */ _NGL_GLYPH(0x1F,0x04,0x04,0x04,0x04),
    /* U */ _NGL_GLYPH(0x11,0x11,0x11,0x11,0x1F),
    /* V */ _NGL_GLYPH(0x11,0x11,0x11,0x0A,0x04),
    /* W */ _NGL_GLYPH(0x11,0x11,0x15,0x15,0x0A),
    /* X */ _NGL_GLYPH(0x11,0x0A,0x04,0x0A,0x11),
    /* Y */ _NGL_GLYPH(0x11,0x0A,0x04,0x04,0x04),
    /* Z */ _NGL_GLYPH(0x1F,0x08,0x04,0x02,0x1F),

    /* [ */ _NGL_GLYPH(0x03,0x01,0x01,0x01,0x03),
    /* \ */ _NGL_GLYPH(0x01,0x02,0x04,0x08,0x10),
    /* ] */ _NGL_GLYPH(0x03,0x02,0x02,0x02,0x03),
    /* ^ */ _NGL_GLYPH(0x04,0x0A,0x00,0x00,0x00),
    /* _ */ _NGL_GLYPH(0x00,0x00,0x00,0x00,0x1F),
    /* ` */ _NGL_GLYPH(0x02,0x04,0x00,0x00,0x00),

    /* a */ _NGL_GLYPH(0x00,0x00,0x07,0x05,0x0F),
    /* b */ _NGL_GLYPH(0x01,0x01,0x07,0x05,0x07),
    /* c */ _NGL_GLYPH(0x00,0x00,0x07,0x01,0x07),
    /* d */ _NGL_GLYPH(0x04,0x04,0x07,0x05,0x07),
    /* e */ _NGL_GLYPH(0x00,0x00,0x07,0x03,0x07),
    /* f */ _NGL_GLYPH(0x03,0x01,0x03,0x01,0x01),
    /* g */ _NGL_GLYPH(0x00,0x00,0x03,0x02,0x03),
    /* h */ _NGL_GLYPH(0x01,0x01,0x07,0x05,0x05),
    /* i */ _NGL_GLYPH(0x01,0x00,0x01,0x01,0x03),
    /* j */ _NGL_GLYPH(0x02,0x00,0x02,0x02,0x03),
    /* k */ _NGL_GLYPH(0x01,0x01,0x05,0x03,0x05),
    /* l */ _NGL_GLYPH(0x01,0x01,0x01,0x01,0x03),
    /* m */ _NGL_GLYPH(0x00,0x00,0x0F,0x15,0x15),
    /* n */ _NGL_GLYPH(0x00,0x00,0x03,0x05,0x05),
    /* o */ _NGL_GLYPH(0x00,0x00,0x07,0x05,0x07),
    /* p */ _NGL_GLYPH(0x00,0x00,0x03,0x03,0x01),
    /* q */ _NGL_GLYPH(0x00,0x00,0x03,0x03,0x02),
    /* r */ _NGL_GLYPH(0x00,0x00,0x07,0x01,0x01),
    /* s */ _NGL_GLYPH(0x00,0x00,0x06,0x02,0x03),
    /* t */ _NGL_GLYPH(0x00,0x02,0x07,0x02,0x06),
    /* u */ _NGL_GLYPH(0x00,0x00,0x05,0x05,0x07),
    /* v */ _NGL_GLYPH(0x00,0x00,0x05,0x05,0x02),
    /* w */ _NGL_GLYPH(0x00,0x00,0x15,0x15,0x0A),
    /* x */ _NGL_GLYPH(0x00,0x00,0x05,0x02,0x05),
    /* y */ _NGL_GLYPH(0x00,0x00,0x05,0x02,0x01),
    /* z */ _NGL_GLYPH(0x00,0x00,0x07,0x02,0x07),

    /* { */ _NGL_GLYPH(0x06,0x02,0x01,0x02,0x06),
    /* | */ _NGL_GLYPH(0x04,0x04,0x04,0x04,0x04),
    /* } */ _NGL_GLYPH(0x03,0x02,0x04,0x02,0x03),
    /* ~ */ _NGL_GLYPH(0x00,0x02,0x15,0x08,0x00),
};

ngl_error_t ngl_load_glyphs(ngl_font_t *font, const u32 *glyphs) {
    if (!font) return ERR_INVALID_PTR;

    if (font->w == 0 || font->w > 5) font->w = _NGL_DEFAULT_GLYPH_W;
    if (font->h == 0 || font->h > 5) font->h = _NGL_DEFAULT_GLYPH_H;
    if (font->hpad == 0) font->hpad = _NGL_DEFAULT_GLYPH_HPAD;
    if (font->vpad == 0) font->vpad = _NGL_DEFAULT_GLYPH_VPAD;

    if (glyphs == NULL) font->glyphs = _NGL_DEFAULT_GLYPHS;
    else                font->glyphs = glyphs;


    return ERR_SUCCESS;   
}

ngl_error_t ngl_draw_glyph(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color, char glyph) {
    ngl_screen_t *screen = &api->screen;
    ngl_font_t font = api->font;

    if (!screen || !screen->next.chars || !screen->next.colors || !font.glyphs) return ERR_INVALID_PTR;
    if (x >= screen->w || y >= screen->h || font.w > screen->w - x || font.h > screen->h - y) return ERR_INVALID_SIZE;

    /* All the alphanumeric Symbols in the ASCII Table. */
    char startc = '!';
    char endc = '~';

    if (glyph < startc || glyph > endc) {
        return ERR_INVALID_SIZE;
    }

    u32 glyph_index = (u32)(glyph - startc);

    u32 bits = font.glyphs[glyph_index];
    u32 gy, gx;
    for (gy = 0; gy < font.h; ++gy) {
        for (gx = 0; gx < font.w; ++gx) {
            if ((bits >> (gy * font.w + gx)) & 1u) {
                u32 screen_i = ngl_idx(x + gx, y + gy, screen->w);

                screen->next.chars[screen_i] = c;
                screen->next.colors[screen_i] = color;
            }
        }
    }

    
    return ERR_SUCCESS;
}

ngl_error_t ngl_draw_text(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color, const char *str) {
    ngl_screen_t *screen = &api->screen;
    ngl_font_t font = api->font;

    if (!screen || !screen->next.chars || !screen->next.colors || !font.glyphs) return ERR_INVALID_PTR;
    if (!str) return ERR_INVALID_PTR;

    ngl_error_t err = ERR_SUCCESS;
    u32 cx = x, cy = y;
    while (*str != '\0') {
        /* Basic ASCII escape Codes. */
        if (*str == '\n') { /* New line. */
            cx = x;
            cy += font.h + font.vpad;
            str++;
            continue;
        } 
        if (*str == '\t')      cx += 4 * font.w + font.hpad;  /* Horizontal Tab. */
        else if (*str == '\v') cy += 4 * font.h + font.vpad;  /* Vertical Tab.   */
        else if (*str == '\r') cx = x;                        /* Cariage Return. */
        else if (*str == ' ') cx += font.w + font.hpad;
        else {

            err |= ngl_draw_glyph(api,  cx, cy, c, color, *str);
            if (err) {
                str++;
                continue;
            }

            cx += font.w + font.hpad;
            /* Wrap around. */
            if (cx + font.w > screen->w) {
                cx = x;
                cy += font.h + font.vpad;
            }

            if (cy + font.h > screen->h) {
                break;
            }
        }


        str++;
    }
    return err;
}

ngl_error_t ngl_draw_text_fmt(ngl_t *api, u32 x, u32 y, char c, ngl_color_t color, const char *format, ...) {
    va_list args;
    va_start(args, format);

    va_list tmp_args;
    va_copy(tmp_args, args);

    int length = vsnprintf(NULL, 0, format, tmp_args);
    va_end(tmp_args);

    if (length < 0) {
        va_end(args);
        return ERR_INVALID_SIZE;
    }

    char *buf = (char*)NGL_MALLOC(((size_t)length + 1) * sizeof(char));

    if (!buf) {
        va_end(args);
        return ERR_FAILED_MALLOC;
    }

    if (vsnprintf(buf, (size_t)length + 1, format, args) < 0) {
        NGL_FREE(buf);
        va_end(args);
        return ERR_INVALID_SIZE;
    }

    va_end(args);

    ngl_error_t err = ngl_draw_text(api, x, y, c, color, buf);

    NGL_FREE(buf);
    return err;
}

#endif /* NGL_NO_FONTS */




#ifndef NGL_NO_MATH
/*
 * +-------------------------------------------------------------------------+
 * |                            ngl math Implementation.                     |
 * +-------------------------------------------------------------------------+
 */

NGL_MATHDEF ngl_vec2_t ngl_vec2_add(ngl_vec2_t a, ngl_vec2_t b) {
    return ngl_vec2(a.x + b.x, a.y + b.y);
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_sub(ngl_vec2_t a, ngl_vec2_t b) {
    return ngl_vec2(a.x - b.x, a.y - b.y);
}


NGL_MATHDEF ngl_vec2_t ngl_vec2_mul(ngl_vec2_t a, ngl_vec2_t b) {
    return ngl_vec2(a.x * b.x, a.y * b.y);
}

NGL_MATHDEF ngl_float ngl_vec2_dot(ngl_vec2_t a, ngl_vec2_t b) {
    return a.x * b.x + a.y * b.y;
}

NGL_MATHDEF ngl_float ngl_vec2_cross(ngl_vec2_t a, ngl_vec2_t b) {
    return a.x * b.y - a.y * b.x;
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_div(ngl_vec2_t a, ngl_vec2_t b) {
    return ngl_vec2(a.x / b.x, a.y / b.y);
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_sqrt(ngl_vec2_t vec) {
    return ngl_vec2(NGL_SQRT(vec.x), NGL_SQRT(vec.y));
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_scale(ngl_vec2_t vec, ngl_float scalar) {
    return ngl_vec2(vec.x * scalar, vec.y * scalar);
}

NGL_MATHDEF ngl_float ngl_vec2_len(ngl_vec2_t vec) {
    return NGL_SQRT(vec.x * vec.x + vec.y * vec.y);
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_normalize(ngl_vec2_t vec) {
    ngl_float length = ngl_vec2_len(vec);

    if (length == 0.0) {
        return ngl_vec2(0.0, 0.0);
    }

    return ngl_vec2(vec.x / length, vec.y / length);
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_rot90cw(ngl_vec2_t vec) {
    return ngl_vec2(-vec.y, vec.x);
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_rot90ccw(ngl_vec2_t vec) {
    return ngl_vec2(vec.y, -vec.x);
}

NGL_MATHDEF ngl_vec2_t ngl_vec2_rot(ngl_vec2_t vec, ngl_float angle) {
    ngl_float c = NGL_COS(angle);
    ngl_float s = NGL_SIN(angle);

    ngl_vec2_t res = {0};

    res.x = vec.x * c - vec.y * s;
    res.y = vec.x * s + vec.y * c;

    return res;
}


#endif /* NGL_NO_MATH */
#endif /* NGL_IMPLEMENTATION */


#ifndef _NGL_PREFIX
#define _NGL_PREFIX
#ifndef NGL_UNSTRIP_PREFIX

/*
 * +-------------------------------------------------------------------------+
 * |                               Strip Prefices                            |
 * +-------------------------------------------------------------------------+
 */

typedef ngl_error_t            error_t;
typedef ngl_screen_t           screen_t;
typedef ngl_color_t            color_t;

#define WHITE                  NGL_WHITE
#define BLACK                  NGL_BLACK
#define RED                    NGL_RED
#define GREEN                  NGL_GREEN
#define YELLOW                 NGL_YELLOW
#define BLUE                   NGL_BLUE
#define MAGENTA                NGL_MAGENTA
#define CYAN                   NGL_CYAN

#define error_to_string        ngl_error_to_string
#define idx                    ngl_idx

#define delay                  ngl_delay
#define get_ms                 ngl_get_ms
#define clear_screen           ngl_clear_screen

#define get_term_size          ngl_get_term_size

/* We already use 'color' as a Parameter in many Funcitons, so we don't strip the prefix here. */
/* #define color                  ngl_color */

#define init_screen            ngl_init_screen
#define screen_new             ngl_screen_new
#define destroy_screen         ngl_destroy_screen

#define print_screen           ngl_print_screen
#define fill_bg                ngl_fill_bg

#define set_pixel              ngl_set_pixel
#define draw_line              ngl_draw_line
#define draw_rect              ngl_draw_rect

#define draw_sprite            ngl_draw_sprite
#define draw_screen_borders    ngl_draw_screen_borders

#ifndef NGL_NO_INPUT

typedef ngl_input_ctx_t        input_ctx_t;
typedef ngl_key_state_t        key_state_t;

#define is_key_down            ngl_is_key_down
#define is_key_pressed_repeat  ngl_is_key_pressed_repeat
#define is_key_released        ngl_is_key_released

#define init_input             ngl_init_input
#define input_new              ngl_input_new
#define destroy_input          ngl_destroy_input

#define get_key_state          ngl_get_key_state
#define get_keyboard_state     ngl_get_keyboard_state

#endif /* NGL_NO_INPUT */

#ifndef NGL_NO_FONTS

#define load_glyphs            ngl_load_glyphs
#define draw_glyph             ngl_draw_glyph
#define draw_text              ngl_draw_text
#define draw_text_fmt          ngl_draw_text_fmt

typedef ngl_font_t             font_t;

#endif /* NGL_NO_FONTS */

#ifndef NGL_NO_MATH

#define vec2_t         ngl_vec2_t
#define vec2           ngl_vec2

#define vec2_add       ngl_vec2_add
#define vec2_sub       ngl_vec2_sub
#define vec2_dot       ngl_vec2_dot
#define vec2_cross     ngl_vec2_cross
#define vec2_div       ngl_vec2_div
#define vec2_sqrt      ngl_vec2_sqrt
#define vec2_scale     ngl_vec2_scale
#define vec2_len       ngl_vec2_len
#define vec2_normalize ngl_vec2_normalize
#define vec2_rot90cw   ngl_vec2_rot90cw
#define vec2_rot90ccw  ngl_vec2_rot90ccw
#define vec2_mag       ngl_vec2_mag
#define vec2_mul       ngl_vec2_mul
#define vec2_rot       ngl_vec2_rot

#define deg_to_rad     ngl_deg_to_rad

#endif /* NGL_NO_MATH */
#endif /* NGL_UNSTRIP_PREFIX */
#endif /* _NGL_PREFIX*/
