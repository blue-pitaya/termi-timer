#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include <sys/time.h>

typedef enum {
    ERR_OK = 0,
    ERR_FAIL = 1,
    ERR_EXIT = 2,
} Err;

typedef enum {
    STATE_INIT,
    STATE_RUNNING,
    STATE_PAUSED,
} State;

typedef struct {
    int terminal_w;
    int terminal_h;
    WINDOW *window;
    struct timeval start;
    long long total_elapsed_ms;
    State state;
} UI;

Err UI_init(UI *);
Err UI_handle_key(UI *, int key);
void UI_render(UI *);
void UI_destroy(UI *ui);
void UI_resize(UI *ui);

#endif
