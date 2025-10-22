#include "UI.h"
#include <bits/types/struct_timeval.h>
#include <dirent.h>
#include <linux/limits.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

const int digits[10][7] = {{1, 0, 1, 1, 1, 1, 1}, {0, 0, 0, 0, 1, 0, 1},
                           {1, 1, 1, 0, 1, 1, 0}, {1, 1, 1, 0, 1, 0, 1},
                           {0, 1, 0, 1, 1, 0, 1}, {1, 1, 1, 1, 0, 0, 1},
                           {1, 1, 1, 1, 0, 1, 1}, {1, 0, 0, 0, 1, 0, 1},
                           {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 0, 1}};

const int line_size_x = 15;
const int line_size_y = 13;
const int space = 2;
const int delimeter_w = 1;

const int timer_display_w = 6 * line_size_x + 2 * delimeter_w + 7 * space;
const int timer_display_h = line_size_y;

const char *state_init_label = "Init";
const char *state_running_label = "Running";
const char *state_paused_label = "Paused";

Err load_terminal_size(UI *ui) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return ERR_FAIL;
    }
    ui->terminal_w = w.ws_col;
    ui->terminal_h = w.ws_row;

    return ERR_OK;
}

Err UI_init(UI *ui) {
    ui->state = STATE_INIT;
    gettimeofday(&ui->start, NULL);

    int err;
    if ((err = load_terminal_size(ui)) != ERR_OK) {
        return err;
    }

    ui->window = newwin(ui->terminal_h / 2, ui->terminal_w / 2,
                        ui->terminal_h / 4, ui->terminal_w / 4);

    return ERR_OK;
};

Err UI_handle_key(UI *ui, int key) {
    switch (key) {
    case 'q': {
        return ERR_EXIT;
    }
    case ' ': {
        switch (ui->state) {
        case STATE_INIT: {
            ui->state = STATE_RUNNING;
            ui->total_elapsed_ms = 0;
            gettimeofday(&ui->start, NULL);
            break;
        }
        case STATE_RUNNING: {
            ui->state = STATE_PAUSED;

            struct timeval end;
            gettimeofday(&end, NULL);
            int elapsed_ms = ui->total_elapsed_ms;
            elapsed_ms += (end.tv_sec - ui->start.tv_sec) * 1000;
            elapsed_ms += (end.tv_usec - ui->start.tv_usec) / 1000;

            ui->total_elapsed_ms = elapsed_ms;
            break;
        }
        case STATE_PAUSED: {
            ui->state = STATE_RUNNING;
            gettimeofday(&ui->start, NULL);
            break;
        }
        }
        break;
    }
    }

    return ERR_OK;
}

void render_digit(UI *ui, int start_y, int start_x, char d) {
    for (int i = 0; i < 7; i++) {
        if (digits[d][i] == 1) {
            switch (i) {
            case 0:
                for (int j = 0; j < line_size_x; j++) {
                    mvwprintw(ui->window, start_y, start_x + j, "█");
                }
                break;
            case 1:
                for (int j = 0; j < line_size_x; j++) {
                    mvwprintw(ui->window, start_y + line_size_y / 2,
                              start_x + j, "█");
                }
                break;
            case 2:
                for (int j = 0; j < line_size_x; j++) {
                    mvwprintw(ui->window, start_y + line_size_y - 1,
                              start_x + j, "█");
                }
                break;
            case 3:
                for (int j = 0; j < (line_size_y / 2 + line_size_y % 2); j++) {
                    mvwprintw(ui->window, start_y + j, start_x, "█");
                }
                break;
            case 4:
                for (int j = 0; j < (line_size_y / 2 + line_size_y % 2); j++) {
                    mvwprintw(ui->window, start_y + j,
                              start_x + line_size_x - 1, "█");
                }
                break;
            case 5:
                for (int j = 0; j < (line_size_y / 2 + line_size_y % 2); j++) {
                    mvwprintw(ui->window, start_y + j + line_size_y / 2,
                              start_x, "█");
                }
                break;
            case 6:
                for (int j = 0; j < (line_size_y / 2 + line_size_y % 2); j++) {
                    mvwprintw(ui->window, start_y + j + line_size_y / 2,
                              start_x + line_size_x - 1, "█");
                }
                break;
            }
        }
    }
}

void render_delimeter(UI *ui, int start_y, int start_x) {
    mvwprintw(ui->window, start_y + line_size_y / 2 - 2, start_x, "█");
    mvwprintw(ui->window, start_y + line_size_y / 2 + 2, start_x, "█");
}

void UI_render(UI *ui) {
    box(ui->window, 0, 0);

    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    switch (ui->state) {
    case STATE_INIT: {
        break;
    }
    case STATE_RUNNING: {
        struct timeval end;
        gettimeofday(&end, NULL);
        int elapsed_ms = ui->total_elapsed_ms;
        elapsed_ms += (end.tv_sec - ui->start.tv_sec) * 1000;
        elapsed_ms += (end.tv_usec - ui->start.tv_usec) / 1000;

        hours = (elapsed_ms / 1000) / 3600;
        minutes = ((elapsed_ms / 1000) / 60) % 60;
        seconds = (elapsed_ms / 1000) % 60;
        break;
    }
    case STATE_PAUSED: {
        int elapsed_ms = ui->total_elapsed_ms;

        hours = (elapsed_ms / 1000) / 3600;
        minutes = ((elapsed_ms / 1000) / 60) % 60;
        seconds = (elapsed_ms / 1000) % 60;
        break;
    }
    }

    int window_h, window_w;
    getmaxyx(ui->window, window_h, window_w);

    int offset_y = window_h / 2 - (timer_display_h / 2);
    int offset_x = window_w / 2 - (timer_display_w / 2);

    const char *label;
    switch (ui->state) {
    case STATE_INIT:
        label = state_init_label;
        break;
    case STATE_RUNNING:
        label = state_running_label;
        break;
    case STATE_PAUSED:
        label = state_paused_label;
        break;
    }

    mvwprintw(ui->window, 1, 1, "Status: %s", label);

    offset_x += 0;
    render_digit(ui, offset_y, offset_x, (hours / 10) % 10);

    offset_x += line_size_x + space;
    render_digit(ui, offset_y, offset_x, hours % 10);

    offset_x += line_size_x + space;
    render_delimeter(ui, offset_y, offset_x);

    offset_x += delimeter_w + space;
    render_digit(ui, offset_y, offset_x, (minutes / 10) % 10);

    offset_x += line_size_x + space;
    render_digit(ui, offset_y, offset_x, minutes % 10);

    offset_x += line_size_x + space;
    render_delimeter(ui, offset_y, offset_x);

    offset_x += delimeter_w + space;
    render_digit(ui, offset_y, offset_x, (seconds / 10) % 10);

    offset_x += line_size_x + space;
    render_digit(ui, offset_y, offset_x, seconds % 10);
}

void UI_destroy(UI *ui) {
    wborder(ui->window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(ui->window);
    delwin(ui->window);
}
