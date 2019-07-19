#include "pch.h"
#include "curses.h"
#include "GameSudoku.h"
#include <ctime>
#include <string>
#include <vector>

GameSudoku::GameSudoku() {
	e.seed(time(NULL));
	start();
}  //初始化

int GameSudoku::getStatus() {
	return status;
}  //获取当前游戏状态

void moveCur(int loc) {
	int x = loc / ROW, y = loc % COL;
	move(OFFSET_Y + 2 * x + 1, (COLS - WIDTH) / 2 + 4 * (y + 1) + y / 3 - 1);
}

void initStack(std::stack<std::pair<int, int> > &s) {
	while (!s.empty())
		s.pop();
}  //清空栈

void initMap(int (*m)[COL]) {
	for (int i = 0; i < ROW; ++i)
		for (int j = 0; j < COL; ++j)
			m[i][j] = 0;
}

void GameSudoku::processInput() {
	char cmd = getch();
	if (cmd >= 'a' && cmd <= 'z')
		cmd -= 32;
	if (status == S_NORMAL) {
		switch (cmd) {
			case 'A':
				if (now) {
					--now;
					moveCur(now);
				}
				break;
			case 'S':
				if (now < (ROW - 1) * COL) {
					now += COL;
					moveCur(now);
				}
				break;
			case 'D':
				if (now < ROW * COL - 1) {
					++now;
					moveCur(now);
				}
				break;
			case 'W':
				if (now >= COL) {
					now -= COL;
					moveCur(now);
				}
				break;
			case 'Z':
				if (!presteps.empty())
					loadStack(presteps, recovery);
				break;
			case 'X':
				if (!recovery.empty())
					loadStack(recovery, presteps);
				break;
			case 'C':
				for (int i = 0; i < ROW; ++i)
					for (int j = 0; j < COL; ++j)
						qMap[i][j] = iMap[i][j];
				initStack(presteps);
				initStack(recovery);
		}
		if (cmd >= '1' && cmd <= '9') {
			int num = cmd - '0', x = now / ROW, y = now % COL;
			int pre = qMap[x][y];
			qMap[x][y] = num;
			if (isUnique(x, y)) {
				std::pair<int, int> p(now, pre);
				presteps.push(p);
				initStack(recovery);
				if (!pre) {
					--remain;
					if (!remain) {
						status = S_COMPLETE;
						curs_set(0);
					}
				}
			}
			else {
				qMap[x][y] = pre;
				beep();
			}
		}
	}
	switch (cmd) {
		case 'E':
			if (status == S_NORMAL) {
				status = S_ANSWER;
				curs_set(0);
			}
			else if (status == S_ANSWER) {
				status = S_NORMAL;
				curs_set(2);
			}
			break;
		case 'R':
			start();
			break;
		case 'Q':
			status = S_QUIT;
	}
}

void GameSudoku::updateMap(int loc, int num) {
	qMap[loc / ROW][loc % COL] = num;
}  //更新数独表

void GameSudoku::loadStack(std::stack<std::pair<int, int> > &s1, std::stack<std::pair<int, int> > &s2) {
	int loc = s1.top().first;
	std::pair<int, int> p(loc, qMap[loc / ROW][loc % COL]);
	s2.push(p);
	updateMap(loc, s1.top().second);
	s1.pop();
}  //读取栈顶并栈间转移，s1为读取栈，s2为输出栈

void GameSudoku::draw() {
	for (int i = 0; i < ROW; ++i)
		for (int j = 0; j < COL; ++j)
			if (status == S_ANSWER)
				mvprintw(OFFSET_Y + 2 * i + 1, (COLS - WIDTH) / 2 + 4 * (j + 1) + j / 3 - 1, std::to_string(aMap[i][j]).data());
			else {
				if (qMap[i][j])
					mvprintw(OFFSET_Y + 2 * i + 1, (COLS - WIDTH) / 2 + 4 * (j + 1) + j / 3 - 1, std::to_string(qMap[i][j]).data());
				else
					mvprintw(OFFSET_Y + 2 * i + 1, (COLS - WIDTH) / 2 + 4 * (j + 1) + j / 3 - 1, " ");
			}
	moveCur(now);
	//其他文本绘制
	if (status == S_COMPLETE) {
		std::string strWin = "CONGRATULATION!";
		mvprintw(OFFSET_Y + 23, (COLS - strWin.size()) / 2, strWin.data());
	}
	refresh();
}  //填充数字

void GameSudoku::drawFrame() {
	clear();
	for (int i = 0; i <= ROW; ++i) {
		move(OFFSET_Y + 2 * i, (COLS - WIDTH) / 2);
		for (int j = 0; j < COL; ++j) {
			addch('+');
			if (!(j % 3))
				addch('+');
			if (i % 3)
				addstr("---");
			else
				addstr("===");
		}
		addstr("++");  //横线边框绘制
		if (i != ROW) {
			move(OFFSET_Y + 2 * i + 1, (COLS - WIDTH) / 2);
			for (int j = 0; j < COL; ++j) {
				if (!(j % 3))
					addch('|');
				addstr("|   ");
			}
			addstr("||");
		}  //竖线边框绘制
	}
	//其他文本绘制
	std::string strTitle = "SUDOKU",
		strHelp1 = "W(UP),S(DOWN),A(LEFT),D(RIGHT),1-9(FILL NUM),R(RESTART),Q(QUIT)",
		strHelp2 = "E(SWITCH ANSWER/QUESTION),Z(CANCEL),X(RECOVERY),C(CLEAR)";
	mvprintw(OFFSET_Y / 2, (COLS - strTitle.size()) / 2, strTitle.data());
	mvprintw(OFFSET_Y + 20, (COLS - strHelp1.size()) / 2, strHelp1.data());
	mvprintw(OFFSET_Y + 21, (COLS - strHelp2.size()) / 2, strHelp2.data());
	refresh();
}  //绘制边框，使用双线区分各九宫格

void GameSudoku::start() {
	drawFrame();
	curs_set(2);
	std::uniform_int_distribution<int> r1(MIN, MAX);
	std::uniform_int_distribution<int> r2(0, ROW-1);
	status = S_NORMAL;
	//生成一个完整的9x9数独
	for (int i = 0; i < COL; ++i)
		aMap[0][i] = i + 1;
	std::shuffle(aMap[0], aMap[0] + 9, e);
	for (int i = 1; i < ROW; ++i)
		for (int j = 0; j < COL; ++j)
			aMap[i][j] = 0;
	init(9);
	initMap(qMap);
	initMap(iMap);
	//将一定数量的数字显示
	int t = r1(e);
	for (int i = 0; i < t;) {
		int x = r2(e), y = r2(e);
		if (!qMap[x][y]) {
			qMap[x][y] = iMap[x][y] = aMap[x][y];
			++i;
		}
	}
	for (now = 0; now < ROW*COL; ++now)
		if (!qMap[now / ROW][now%COL])
			break;
	remain = ROW * COL - t;
	initStack(presteps);
	initStack(recovery);
}  //开始一局数独

bool GameSudoku::init(int n) {
	if (n == ROW * COL)
		return true;
	int x = n / 9, y = n % 9;
	std::vector<bool> choice(ROW, false);
	std::vector<int> tryList;  //可填值，顺序排列
	for (int i = 0; i < ROW; ++i)
		if (aMap[i][y])
			choice[aMap[i][y] - 1] = true;
	for (int j = 0; j < COL; ++j)
		if (aMap[x][j])
			choice[aMap[x][j] - 1] = true;
	for (int i = x / 3 * 3; i < x / 3 * 3 + 3; ++i)
		for (int j = y / 3 * 3; j < y / 3 * 3 + 3; ++j)
			if (aMap[i][j])
				choice[aMap[i][j] - 1] = true;
	for (int i = 0; i < ROW; ++i)
		if (!choice[i])
			tryList.push_back(i + 1);
	while (!tryList.empty()) {
		std::uniform_int_distribution<int> r(0, tryList.size() - 1);
		int loc = r(e);
		aMap[x][y] = tryList[loc];
		if (init(n + 1))
			return true;
		aMap[x][y] = 0;
		tryList.erase(tryList.begin() + loc);
	}
	return false;
}  //生成一个数独

bool GameSudoku::isUnique(int x, int y) {
	for (int i = 0; i < ROW; ++i)
		if (i != x && qMap[i][y] == qMap[x][y])
			return false;
	for (int j = 0; j < COL; ++j)
		if (j != y && qMap[x][j] == qMap[x][y])
			return false;
	for (int i = x / 3 * 3; i < x / 3 * 3 + 3; ++i)
		for (int j = y / 3 * 3; j < y / 3 * 3 + 3; ++j)
			if ((i != x || j != y) && qMap[i][j] == qMap[x][y])
				return false;
	return true;
}  //检查行、列、九宫格唯一性