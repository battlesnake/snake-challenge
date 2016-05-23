CFLAGS := -std=c99 -g -Og -ffunction-sections -fdata-sections -Wall -Wextra -pedantic -Werror
LFLAGS := -flto -Wl,--gc-sections -lcurses

ifeq ($(mode),small)
# Challenge: can you get it below 4K while still executable on a generic x86_64?
CFLAGS := -Dmain=_start -Qn -nostartfiles -s -Os -flto -fno-stack-protector -omit-frame-poiner -fno-pic -fno-pie -m32 -march=i386 -mtune=i386 $(filter-out -g -Og, $(CFLAGS))
endif

target := snake
units := main snake

$(target): $(units:%=%.o)
	gcc $(CFLAGS) $^ $(LFLAGS) -o $@
ifeq ($(mode),small)
	strip -s -R .comment -R .note.ABI-tag -R .note.gnu.build-id $@
	size $@
	du -h $@
endif

%.o: %.c
	gcc -c $(CFLAGS) $^ -o $@

clean:
	rm -f -- *.o $(target)
