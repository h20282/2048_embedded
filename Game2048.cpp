#include "Game2048.h"
#include "Point.h"

#include <ctime>
#include <pthread.h>

#include <numeric>
#include <iostream>
#include <map>
#include <algorithm>

namespace{

inline int GetPos(int x, int y) { return y*kSize+x; }
inline int GetPos(Point p) { return GetPos(p.x, p.y); }

/**
 * @brief {0, 4, 2, 2} -> {4, 4, 0, 0}
 *
 * @param arr    kSize 个指向一行|一列数字的指针 
 */
bool DoMove(int **arr){
	bool can_move = false;
	for (int i=0; i<kSize; ++i) {
		int &curr = *arr[i];
		for (int j=i+1; j<kSize; j++){
			if (*arr[j] == curr){
				*arr[j] = 0;
				curr *= 2;

				can_move = true;
				break;
			}
			if (*arr[j] != 0) {
				break;
			}
		}
	}
	
	for (int t=0; t<kSize; ++t) {
		for (int i=0; i<kSize-1; ++i) {
			int &curr = *arr[i];
			if (curr==0) {
				curr = *arr[i+1];
				*arr[i+1] = 0;
				if (curr!=0) {
					can_move = true;
				}
			}
		}
	}
	return can_move;
}

/**
 * @brief 对begin为起点，adder为方向的kSize个数字进行移动，移动方向为朝向begin
 * 	        
 * 	        坐标系：  +----> x
 * 	                  | 
 * 	                  |  1 2 3 4
 * 	                 y|  5 6 7 8
 * 	                  v  ...
 * 
 * @param base 所有数字的首地址，数字从上到下，从左到右排列
 *
 * @param begin 起始点
 *
 * @param adder 迭代方向
 */
bool DoMove(int *base, Point begin, Point adder) {
	Point p_arr[kSize];
	Point p = begin;
	for (int i=0; i<kSize; ++i, p=p+adder) {
		p_arr[i] = p;
	} 
	int *arr[kSize];
	for (int i=0; i<kSize; ++i) {
		arr[i] = base + GetPos(p_arr[i]);
	}
	return DoMove((int**)&arr);
}


/**
 * @brief 向方向dir移动
 *
 * @param arr 所有数字的首地址
 *
 * @param dir 移动方向
 */
bool DoMove(int *arr, Dir dir) {
	bool can_move = false;
	if (dir==kUp) {
		for (int x=0; x<kSize; ++x) {
			if (DoMove(arr, {x, 0}, {0, 1})) can_move = true;
		}
	} else if (dir==kDown) {
		for (int x=0; x<kSize; ++x) {
			if (DoMove(arr, {x, kSize-1}, {0, -1})) can_move = true;
		}
	} else if (dir==kLeft) {
		for (int y=0; y<kSize; ++y) {
			if (DoMove(arr, {0, y}, {1, 0})) can_move = true;
		}
	} else if (dir==kRight) {
		for (int y=0; y<kSize; ++y) {
			if (DoMove(arr, {kSize-1, y}, {-1, 0})) can_move = true;
		}
	}
	return can_move;
}

}// namespace 




Game2048::Game2048(CallBack on_moved, CallBack1 on_new_item)
            : on_moved_(on_moved),
              on_new_item_(on_new_item){

    srand(time(nullptr));
    Init();
}

void Game2048::Init(){
    score_ = 0;
    step_ = 0;
    history_ = std::stack<Status>();

    Status curr(kSize*kSize);
    for (int i=0; i<2; ++i){
        NewItem(curr);
        history_.push(curr);
        on_moved_(history_.top().data());
    }
}

Game2048::Status Game2048::GetCurr(){
    if (history_.size()==0){
        Init();
    }
    return history_.top();
}

bool Game2048::Move(Dir dir){
    if (history_.size() == 0) {
        Init();
        return false;
    }
    Status curr = history_.top();
    DoMove(curr.data(), dir);
	if (curr != history_.top()) {
        on_moved_(curr.data());
		NewItem(curr);
		history_.push(curr);
        step_++;
        return true;
	}
    return false;
}

bool Game2048::GameOver() {
    for (auto dir : {kUp, kLeft, kRight, kDown}) {
        Status curr = history_.top();
        DoMove(curr.data(), dir);
        if (curr != history_.top()) {
            return false;
        }
    }
    return true;
}

int Game2048::GetScore(){
	if (history_.size()) {
		return accumulate(history_.top().begin(), history_.top().end(), 0);
	} else {
		return 0;
	}
}

int Game2048::GetStep() {
    return step_;
}

void Game2048::Reorder(){
    if (history_.size()){
        Status curr = history_.top();
        sort(curr.begin(), curr.end());
        history_.push(curr);
        on_moved_(curr.data());
    }
}

bool Game2048::GoBack(){
    if (history_.size() > 1){
        history_.pop();
        on_moved_(history_.top().data());
    }
}

void Game2048::ReStart(){
    Init();
}

void Game2048::NewItem(Status& curr){
    std::vector<int> all_0_idx;
    for (int i=0; i < curr.size(); i++){
        if (curr[i]==0){
            all_0_idx.push_back(i);
        }
    }
    if (all_0_idx.size()) {
        int idx = all_0_idx[rand()%all_0_idx.size()];
        curr[idx] = rand()%10==0 ? 4 : 2;
        on_new_item_(idx%4, idx/4, curr[idx]);
    }
}

/*
int main(){

    std::map<char, Dir> char2dir{
        {'w', kUp},
        {'s', kDown},
        {'a', kLeft},
        {'d', kRight},
    };

    Game2048 game;

    while (1) {
        for (int y=0; y < kSize; y++){
            for (int x=0; x < kSize; x++){
                printf("%d  ", game.GetCurr()[y*kSize+x]);
            }
            printf("\n");
            printf("\n");
        }
        char op;
        std::cin>>op;
        if (op == 'b'){
            game.GoBack();
        } else {
            game.Move(char2dir[op]);
        }
    }
    
}
*/
