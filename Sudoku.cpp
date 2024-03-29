#include "pch.h"
#include "curses.h"
#include "GameSudoku.h"

void init() {
	initscr();
	cbreak();
	noecho();
}

void shutdown() {
	endwin();
}

int main()
{
	init();
	GameSudoku game;
	while (game.getStatus() != S_QUIT) {
		game.draw();
		game.processInput();
	}
	shutdown();
	return 0;
}
