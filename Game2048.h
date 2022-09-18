#pragma once

#include <functional>
#include <vector>
#include <stack>

enum Dir{ kUp, kRight, kDown, kLeft };

constexpr int kSize = 4;

class Game2048{
public:
	using Status = std::vector<int>;
	using CallBack = std::function<void(int *)>;
	using CallBack1 = std::function<void(int, int, int)>;
public:	
	Game2048(CallBack on_moved, CallBack1 on_new_item);

	Status GetCurr();
	bool Move(Dir dir);
	bool GoBack();
	int GetScore();
	int GetStep();
	void Reorder();
	void ReStart();
	bool GameOver();

private:
	void Init();
	void NewItem(Status& curr);
	
private:
	int score_ = 0;
	int step_ = 0;
	std::stack<Status> history_;	
	CallBack on_moved_;
	CallBack1 on_new_item_;
};
