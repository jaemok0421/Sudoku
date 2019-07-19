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
	int qMap[ROW][COL];  //��������
	int aMap[ROW][COL];  //��������
	int iMap[ROW][COL];  //��ʼ����
	int now;     //��ǰλ��
	int status;
	int remain;  //ʣ��ո���
	std::stack<std::pair<int, int> > presteps;  //����ջ�����ڲ����ص���λ�ã����֣�
	std::stack<std::pair<int, int> > recovery;  //����ջ�����ڻָ��ص�������λ�ã����֣�
	std::default_random_engine e;
	void start();
	bool init(int);
	void updateMap(int, int);
	void loadStack(std::stack<std::pair<int, int> >&, std::stack<std::pair<int, int> >&);
	bool isUnique(int, int);
	void drawFrame();
};