#pragma once

#include <functional>

#include "img.h"

class Widget {
public:
	using CallBack=std::function<void()>;

public:
	Widget(const char* file_name, int x, int y, CallBack on_clicked);
	void HandleClick();
	bool Contain(int x, int y);
	void Draw();
	int GetWidth();
	int GetHeight();

public:
	Img img_;
	int x_;
	int y_;
	CallBack on_clicked_;
};