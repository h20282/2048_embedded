#pragma once

#include <cmath>

#include <fstream>
#include <vector>

#include "point.h"

enum GL{
	kNearest, kLinear
};

// BGRA8888
class Img{
public:
	Img();
	Img(int w, int h, std::string name);
	Img(const char* file_name);
	char* GetData() ;
	int  GetSize() ;
	int GetPixelSize();
	int GetBytesPerLine();
	int GetWidth();
	int GetHeight();
	std::string GetName();
private:
	int width_;
	int height_;
	std::string name_;
	std::vector<char> data_;
};


/**
 * 将图片Img存储到文件中
 */
bool SaveImg(Img img, std::string file_name);

/**
 * 返回 BGRA
 */
char* GetPixel(Img& img, int x, int y);

/**
 * 以中心点为圆心，将src旋转thelta度
 */
Img Rotate(Img &src, double thelta, GL filtering=kNearest);

/**
 * 返回src的缩放rate倍的结果
 */
Img Scale(Img &src, double rate, GL filtering=kNearest);