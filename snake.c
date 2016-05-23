#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "snake.h"

static void drop_food(struct snake_state *ss)
{
	ss->foodnum++;
	uint8_t x, y;
	do {
		x = rand() % SNAKE_W;
		y = rand() % SNAKE_H;
	} while (ss->grid[y][x] != sc_space);
	ss->food[0] = x;
	ss->food[1] = y;
}

static void gridline_h(struct snake_state *ss, int x1, int x2, int y, int dx)
{
	for (int x = x1; x <= x2; x+=dx) {
		ss->grid[y][x] = sc_wall;
	}
}

static void gridline_v(struct snake_state *ss, int x, int y1, int y2, int dy)
{
	for (int y = y1; y <= y2; y+=dy) {
		ss->grid[y][x] = sc_wall;
	}
}

#define x0 (0)
#define x100 (SNAKE_W - 1)
#define x50 (SNAKE_W / 2)
#define x33 (SNAKE_W / 3)
#define x67 ((SNAKE_W * 2) / 3)
#define x25 (SNAKE_W / 4)
#define x75 ((SNAKE_W * 3) / 4)
#define x20 (SNAKE_W / 5)
#define x40 ((SNAKE_W * 2) / 5)
#define x60 ((SNAKE_W * 3) / 5)
#define x80 ((SNAKE_W * 4) / 5)

#define y0 (0)
#define y100 (SNAKE_H - 1)
#define y50 (SNAKE_H / 2)
#define y33 (SNAKE_H / 3)
#define y67 ((SNAKE_H * 2) / 3)
#define y25 (SNAKE_H / 4)
#define y75 ((SNAKE_H * 3) / 4)
#define y20 (SNAKE_H / 5)
#define y40 ((SNAKE_H * 2) / 5)
#define y60 ((SNAKE_H * 3) / 5)
#define y80 ((SNAKE_H * 4) / 5)

static void init_level(struct snake_state *ss, enum snake_level level)
{
	ss->level = level;
	ss->direction = sd_e;
	ss->head[0] = x50;
	ss->head[1] = y50;
	memset(ss->grid, 0, sizeof(ss->grid));
	ss->grid[y50][x50] = 2;
	ss->grid[y50][x50 - 1] = 1;
	switch (level) {
	case sl_blank:
		break;
	case sl_membranes:
		gridline_v(ss, x33, y0, y100, 2);
		gridline_v(ss, x67, y0, y100, 2);
		break;
	case sl_blocks:
		gridline_v(ss, x33, y33, y67, 1);
		gridline_v(ss, x67, y33, y67, 1);
		break;
	case sl_alternate:
		gridline_v(ss, x20, y0, y67, 1);
		gridline_v(ss, x40, y33, y100, 1);
		gridline_v(ss, x60, y0, y67, 1);
		gridline_v(ss, x80, y33, y100, 1);
		break;
	case sl_spiral:
		gridline_h(ss, x40, x60, y60, 1);
		gridline_v(ss, x60, y40, y60, 1);
		gridline_h(ss, x20, x60, y40, 1);
		gridline_v(ss, x20, y40, y80, 1);
		gridline_h(ss, x20, x80, y80, 1);
		break;
	}
	ss->foodnum = 0;
	drop_food(ss);
}

void snake_init(struct snake_state *ss)
{
	ss->score = 0;
	init_level(ss, sl_blank);
}

static void get_delta(enum snake_direction direction, int *dx, int *dy)
{
	switch (direction) {
	case sd_n:
		*dx = 0, *dy = -1;
		break;
	case sd_e:
		*dx = 1, *dy = 0;
		break;
	case sd_s:
		*dx = 0, *dy = 1;
		break;
	case sd_w:
		*dx = -1, *dy = 0;
		break;
	case sd_same:
	default:
		*dx = 0, *dy = 0;
	}
}

void snake_next_level(struct snake_state *ss)
{
	if (ss->level < MAX_LEVEL) {
		ss->level++;
	} else {
		ss->level = 0;
	}
	init_level(ss, ss->level);
}

static void eat_food(struct snake_state *ss, int *x, int *y)
{
	ss->score += ss->foodnum + (10 * ss->level);
	if (ss->foodnum < 9) {
		drop_food(ss);
	} else {
		snake_next_level(ss);
		*x = ss->head[0];
		*y = ss->head[1];
	}
}

static void move_head(struct snake_state *ss, int x, int y, bool eaten)
{
	ss->grid[y][x] = ss->grid[ss->head[1]][ss->head[0]] + 1;
	for (int y = 0; y < SNAKE_H; y++) {
		for (int x = 0; x < SNAKE_W; x++) {
			if (ss->grid[y][x] > 0) {
				if (eaten) {
					ss->grid[y][x]++;
				} else {
					ss->grid[y][x]--;
				}
			}
		}
	}
	ss->head[0] = x;
	ss->head[1] = y;
}

static void die(struct snake_state *ss)
{
	ss->foodnum = FOOD_DEAD;
}

void snake_command(struct snake_state *ss, enum snake_direction command)
{
	if (ss->foodnum == FOOD_DEAD) {
		return;
	}
	int dx, dy;
	if (command > sd_same) {
		if (command - ss->direction == 2 || ss->direction - command == 2) {
			return;
		}
		ss->direction = command;
	}
	if (command == sd_same) {
		get_delta(ss->direction, &dx, &dy);
	} else {
		get_delta(command, &dx, &dy);
	}
	int x = ss->head[0] + dx;
	int y = ss->head[1] + dy;
	/* Is dead? */
	bool dead = x < 0 || y < 0 || x >= SNAKE_W || y >= SNAKE_H || ss->grid[y][x] != sc_space;
	if (dead) {
		die(ss);
		return;
	}
	/* Eating food? */
	bool eat = ss->food[0] == x && ss->food[1] == y;
	if (eat) {
		eat_food(ss, &x, &y);
	}
	move_head(ss, x, y, eat);
}

void snake_serialise(struct snake_state *ss, struct snake_screen *ser)
{
	ser->score = htonl(ss->score);
	ser->level = ss->level;
	ser->head[0] = ss->head[0];
	ser->head[1] = ss->head[1];
	ser->food[0] = ss->food[0];
	ser->food[1] = ss->food[1];
	ser->foodnum = ss->foodnum;
	memset(ser->cells, 0, sizeof(ser->cells));
	int b = 0;
	uint8_t *t = ser->cells;
	for (int y = 0; y < SNAKE_H; y++) {
		for (int x = 0; x < SNAKE_W; x++) {
			enum snake_cell cell = ss->grid[y][x];
			*t |= (cell ? 1 : 0) << b;
			if (++b == 8) {
				t++;
				b = 0;
			}
		}
	}
}

#define ASCII_SPACE 32
#define ASCII_BLOCK 'o'
#define ASCII_HEAD 'O'
#define ASCII_DEAD 'x'
#define ASCII_FOOD '0'

void snake_render(struct snake_screen *ser, struct snake_view *view)
{
	memset(view, 0, sizeof(*view));
	for (int x = 0; x < SNAKE_W + 2; x++) {
		view->grid[0][x] = ASCII_BLOCK;
		view->grid[SNAKE_H + 1][x] = ASCII_BLOCK;
	}
	for (int y = 0; y < SNAKE_H + 2; y++) {
		view->grid[y][0] = ASCII_BLOCK;
		view->grid[y][SNAKE_W + 1] = ASCII_BLOCK;
		view->grid[y][SNAKE_W + 2] = '\n';
	}
	int b = 0;
	uint8_t *t = ser->cells;
	for (int y = 0; y < SNAKE_H; y++) {
		for (int x = 0; x < SNAKE_W; x++) {
			bool fill = *t >> b & 1;
			view->grid[y + 1][x + 1] = fill ? ASCII_BLOCK : ASCII_SPACE;
			if (++b == 8) {
				t++;
				b = 0;
			}
		}
	}
	bool dead = ser->foodnum == FOOD_DEAD;
	if (dead) {
		view->grid[ser->head[1]+1][ser->head[0]+1] = ASCII_DEAD;
	} else {
		view->grid[ser->head[1]+1][ser->head[0]+1] = ASCII_HEAD;
		view->grid[ser->food[1]+1][ser->food[0]+1] = ASCII_FOOD + ser->foodnum;
	}
	snprintf(view->grid[SNAKE_H + 2], SNAKE_W + 2, "Level: %d, Score: %d %s", ser->level, ntohl(ser->score), dead ? "[DEAD]" : "");
}
