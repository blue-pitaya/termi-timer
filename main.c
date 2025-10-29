#include "src/UI.h"
#include <dirent.h>
#include <locale.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

UI ui;

volatile sig_atomic_t resized = 0;
void handle_winch(int sig) {
    resized = 1;
}

int main(void) {
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    curs_set(0);
    noecho();

    signal(SIGWINCH, handle_winch);

    start_color();
    use_default_colors();
    init_pair(1, COLOR_WHITE, 16);
    init_pair(2, COLOR_BLUE, 16);
    init_pair(3, 16, COLOR_WHITE);

    UI_init(&ui);
    keypad(ui.window, TRUE);
    nodelay(ui.window, TRUE);

    int key = -1;
    Err err = ERR_OK;
    while (true) {
        key = wgetch(ui.window);
        err = UI_handle_key(&ui, key);
        if (err != ERR_OK) {
            break;
        }
        key = -1;

        if (resized) {
            resized = 0;
            endwin();
            refresh();
            clear();
            UI_resize(&ui);
        }

        UI_render(&ui);
        napms(33);
    }

    UI_destroy(&ui);
    endwin();

    return 0;
}
