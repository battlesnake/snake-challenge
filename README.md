Snake challenge
===============

Pre-requisites: C, Linux, Git

This project is set up to build using GNU Make v4.0+, however you can use a different build system provided you use the following flags for GCC/Clang:

	-std=gnu99 -Wall -Wextra -pedantic -Werror

_Other people's code_ is by definition _bad code_.

This challenge requires you to maintain some of my undocumented & uncommented C code, and to cleanly add functionality to it.

**Snake** is a single-player turn-based game inspired by **NIBBLES**, which was a demo program provided with Microsoft QuickBASIC 4.5.
It is similar to the "snake" game on old Nokia phones.

The player controls a snake, which can move up/down/left/right on a square grid.
If the snake intersects itself or a "block" then it dies.
If the snake eats food (by intersecting it) then the snake grows longer, new food appears in another random location, and the player's score increases.

Feel free to play the game a few times to gain an understanding of it before you work with the code.

Complete each challenge in order, do not revert to the original code when starting subsequent challenges (except for challenge X).

After each challenge, COMMIT your changes then tag the commit with the challenge number, for example:

	git add -A
	git commit -m 'Challenge #2 completed'
	git tag challenge-2

You do not need to squash each challenge down to one commit, use tags instead.

Your code must compile with GCC's `-std=gnu99` standard.

## Downloading and running

The challenge is hosted on Github:

	git clone git@github.com:battlesnake/snake-challenge
	cd snake-challenge
	make
	./snake

## Challenge 1: Client server

Objective: Restructure and add features to someone else's code.

Without modifying `snake.c`/`snake.h`, split the program into two separate programs:

 * Server

 * Client

When a client is started, it connects to the server via a TCP socket.
The server maintains a separate game state for each connection.
Each command entered on the client (i.e. keypress) is sent to the server.
The server updates the game state with the given keypress, then serialises the state and returns it to the client.
The client uses this serialised game state to render the game on-screen for the player.

When a client connection is closed (or fails for some other reason, e.g. timeout), the server shall release all resources that were associated with that connection/session.

If possible, spawn a new thread for each client connection, so that multiple games can run simultaneously via multithreading.
Alternatively, run in one thread using `select`/`pselect` to provide event-driven co-operative multitasking instead.

If you use dynamic memory allocation, be sure to also use Valgrind to check for leaks and other memory errors!

## Challenge 2: Low bandwidth

Objective: Implement a streaming encoder/decoder and unit tests for it.

I'm not sure if my 10Gbps internet connection is fast enough to run this incredibly demanding and cutting-edge game.
Reduce the bandwidth requirement of the game.

Implement run-length encoding for the transfer of the serialised game state (i.e. server-to-client data).
To decode a message encoded in this format:

 * Read a byte from the input as signed 8-bit integer (int8\_t / signed char).

 * If it is a negative value `-N`, read the next `N` bytes of data and output them verbatim.

 * If it is a non-negative value `N`, read the next byte and output it `N + 2` times.

Write some simple unit tests for your encoder/decoder (ideally, before you start implementing the encoder/decoder).

## Challenge 3: High latency

Objective: Buffering without over-runs / under-runs or other common causes of bugs.

I hate lag.
Modify the client and server so that I can add several commands (input characters), then send them all at once to the server in one message, receiving one serialised game state in response (after the server has processed all commands).
Now that we can encode many commands in one packet to the server, use the run-length encoding developed in _Challenge #2_ to encode the packets sent to the server.

To start buffering/grouping commands, the player presses "g".
To send the queued command group, the player presses "g" again.
The game returns to normal mode (no command grouping) after that, until the player re-enters it with another "g" press.

Once you have done this, then all packets sent during gameplay will be run-length encoded.

## Challege 4: Cheating bots

Objective: Implement an algorithm correctly.

On the client-side only (do not modify the server code), create a "bot" command which when triggered by the `b` key:

 * generates a series of commands to move the snake to the next food item, without dying.

 * sends these commands to the server.

Hint: Dijkstra / A\* search

## Challenge 5: Versatility

Objective: Parameter processing, sanitising/range-checking, dynamic memory.

One day, I hope that graphics cards will be powerful enough to run this game on grids larger than the current 40x20.
Modify the game so that the user can provide the desired grid size via command-line arguments `-w <width> -h <height>`.
You may use `getopt` for parameter processing.
If no width and/or height is specified, default to the initial values (w/h = 40/20).

## Challenge 6: Compact dimensions

Objective: Range-checking.

Recall the game **Pacman**, where the player can exit the screen on one side and appear on the other side.

Since the game state doesn't include the border columns/rows (they are assumed to be all-block in the original game), you will need to change:

 * the renderer so that it does not draw blocks around the game grid.

 * the level generator, so that it generates a border within the game state itself.

For the first level (initially an empty grid), the holes must be alternating.
Add holes to the vertical (left/right) walls, such that the snake can "wrap" across the screen like Pacman.

	oooooooooo

	o        o

	o        o

	o        o

	o        o
	oooooooooo

If the snake was marked with `x` instead of `o`, then the wrapping would look like this:

	oooooooooo

	o      x o
	xxx    xxx
	o x      o
	xxx   xxx
	o     x  o
	   xxxx
	o        o
	oooooooooo

For levels after the first level, you may implement whatever patterns you want, and also change the levels.
Show off your creativity by re-designing the higher game levels!

## Challenge 7: User-friendly

Objective: Demonstrate understanding of basic UNIX, including fork/dup2/pipe.

Users are lazy and don't read instructions.
Link both your client and server into one executable which when run, uses `fork` to split into separate client and server processes that automatically connect *via pipes*.
When the game ends, both processes must exit.
The server shall receive packets via standard input and respond via standard output, so you will probably need to use `dup2` or `dup3` BEFORE forking the server, and probably figure out a way to define packet boundaries (e.g. escape codes or prefixing each packet with its length).
When the game is started, the pipe/dup/fork must all happen automatically with no extra input required, so that the user can start playing the game on their desired grid size.

## Challenge X: Demoscene

Objective: "Fun".

This challenge is optional, and provided for fun only.  You do not need to do this challenge.  Be sure to commit/tag/push the previous challenges before starting this one.

Checkout the original game source-code and create a new branch:

	git checkout -b demoscene start-here

Without making any changes to the game code that can be observed by someone playing it (e.g. removing strings), see if you can get the executable (`snake`) to fit into 4kB.
To assist you, run the following then have a look in the makefile:

	make clean
	make mode=small

The game must still run on a typical x86\_64 processor, with no observable changes in its behaviour/functionality to the player.

Don't try to cheat e.g. by moving all the code into another binary/dynamic-library and having a <4kB `snake` that simply calls that external file.

I don't know if this challenge is possible, remember it is *optional* and not assessed!

When complete, commit to the demoscene branch and tag the commit as "only-4k", then push.
