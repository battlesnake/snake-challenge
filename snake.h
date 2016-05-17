#pragma once
#include <stdint.h>

#define SNAKE_W 40
#define SNAKE_H 20

enum snake_level { sl_blank = 0, sl_membranes = 1, sl_alternate = 2, sl_blocks = 3, sl_spiral = 4 };

#define MAX_LEVEL sl_spiral

enum snake_direction { sd_same = 0, sd_n = 1, sd_e = 2, sd_s = 3, sd_w = 4 };

enum snake_cell { sc_space = 0, sc_wall = -1, sc_snake_start = 1 };

#define FOOD_DEAD 0

struct snake_state {
	long score;
	enum snake_level level;
	enum snake_direction direction;
	int head[2];
	int food[2];
	int foodnum;
	enum snake_cell grid[SNAKE_H][SNAKE_W];
};

#define SNAKE_PELS ((SNAKE_W)*(SNAKE_H))
#define SNAKE_CELLS (((SNAKE_PELS)+7)/8)

struct __attribute__((__packed__)) snake_screen {
	uint32_t score;
	uint8_t level;
	uint8_t head[2];
	uint8_t food[2];
	uint8_t foodnum;
	uint8_t cells[SNAKE_CELLS];
};

struct snake_view {
	char grid[SNAKE_H + 3][SNAKE_W + 3];
};

void snake_init(struct snake_state *ss);
void snake_command(struct snake_state *ss, enum snake_direction command);
void snake_serialise(struct snake_state *ss, struct snake_screen *ser);
void snake_render(struct snake_screen *ser, struct snake_view *view);
void snake_next_level(struct snake_state *ss);
