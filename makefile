snake: main.c snake.c
	gcc -std=c99 -s -ffunction-sections -fdata-sections -fomit-frame-pointer -flto -fno-stack-protector -Wl,--gc-sections -O3 -Wall -Wextra -pedantic $^ -lcurses -o $@
