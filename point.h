#pragma once

struct Point{
	int x;
	int y;
};

bool operator == (Point a, Point b);
bool operator != (Point a, Point b);
bool operator <  (Point a, Point b);
Point operator + (Point a, Point b);
Point operator - (Point a, Point b);

