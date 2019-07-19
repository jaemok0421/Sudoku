#pragma once
#include "config.h"
#include <random>
#include <stack>
#include <utility>

class GameSudoku {
public:
	GameSudoku();
	int getStatus();
	void processInput();
	void draw();

private:
	int qMap[ROW][COL];  //作答数独
	int aMap[ROW][COL];  //完整数独
	int iMap[ROW][COL];  //初始数独
	int now;     //当前位置
	int status;
	int remain;  //剩余空格数
	std::stack<std::pair<int, int> > presteps;  //操作栈，用于操作回调（位置，数字）
	std::stack<std::pair<int, int> > recovery;  //操作栈，用于恢复回调操作（位置，数字）
	std::default_random_engine e;
	void start();
	bool init(int);
	void updateMap(int, int);
	void loadStack(std::stack<std::pair<int, int> >&, std::stack<std::pair<int, int> >&);
	bool isUnique(int, int);
	void drawFrame();
};