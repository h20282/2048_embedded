#include "Point.h"

bool operator == (Point a, Point b) { return a.x==b.x && a.y==b.y; }
bool operator != (Point a, Point b) { return !(a==b); }
bool operator <  (Point a, Point b) { return a.x==b.x ? a.x<b.x : a.y<b.y; }
Point operator + (Point a, Point b) { return {a.x+b.x, a.y+b.y}; }
Point operator - (Point a, Point b) { return {a.x-b.x, a.y-b.y}; }


