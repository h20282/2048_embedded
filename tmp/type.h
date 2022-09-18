#pragma once

struct Point{
	int x;
	int y;

    Point():x(-1), y(-1){}
};
bool operator == (Point a, Point b);
bool operator != (Point a, Point b);
bool operator <  (Point a, Point b);
Point operator + (Point a, Point b);
Point operator - (Point a, Point b);

struct Resolution{
	int width;
	int height;
}