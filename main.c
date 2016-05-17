#include "snake.h"
#include <curses.h>
#include <signal.h>
#include <stdlib.h>

void sigend(int x)
{
	(void)x;
	endwin();
	exit(0);
}

int main()
{
	struct snake_state state;
	snake_init(&state);
	initscr();
	cbreak();
	keypad(stdscr, true);
	noecho();
	signal(SIGINT, sigend);
	signal(SIGTERM, sigend);
	signal(SIGABRT, sigend);
	signal(SIGSEGV, sigend);
	char c = 0;
	while (c != 'q') {
		struct snake_screen screen;
		snake_serialise(&state, &screen);
		struct snake_view view;
		snake_render(&screen, &view);
		clear();
		printw("\n[WSAD] = Move\n[N] = Cheat to next level\n[X] = Reset\n\n");
		printw("%s", (char *) view.grid);
		refresh();
		c = getch();
		switch (c) {
		case 'w': snake_command(&state, sd_n); break;
		case 's': snake_command(&state, sd_s); break;
		case 'a': snake_command(&state, sd_w); break;
		case 'd': snake_command(&state, sd_e); break;
		case 'n': snake_next_level(&state); break;
		case 'x': snake_init(&state); break;
		default: continue;
		}
	}
	endwin();
	return 0;
}
