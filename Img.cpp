#include "Img.h"

#include <vector>
#include <iostream>

namespace{
inline int Align4(int n){
	return ((n+3)>>2 ) << 2;
}
}

Img::Img() {
}

Img::Img(int w, int h, std::string name)
		:width_(w), height_(h), name_(name) {
	data_.resize(w*h*4);
}

Img::Img(const char* file_name) : name_(file_name) {
	std::ifstream file(file_name, std::ios::in|std::ios::binary);
	if (!file.is_open()){
		printf("open `%s` failed!\n", file_name);
		return;
	}
	auto l = file.tellg();
	file.seekg(0, std::ios::end);
	auto file_size = file.tellg() - l;
	file.seekg(0, std::ios::beg);
	std::vector<char> buff(file_size, 0);
	file.read(buff.data(), file_size);

	for (int i=0; i<54; i++) {
		printf("%03d[%x] ", buff[i], buff[i]);
		if (i%8==0)	printf("\n");
	}
	putchar('\n');

	this->width_ = *reinterpret_cast<int*>(buff.data() + 0x12);
	this->height_ = *reinterpret_cast<int*>(buff.data() + 0x16);
	uint16_t src_pix_size = *reinterpret_cast<uint16_t*>(buff.data() + 0x1c) / 8;

	this->data_.resize(width_*height_*GetPixelSize());

	printf("file %s: %dbytes per pixel, [%d*%d], %dbytes->%dbytes\n", file_name, src_pix_size, width_, height_, buff.size(), data_.size());

	for (int y=0; y<height_; ++y) {
		for (int x=0; x<width_; ++x) {
			int offset_target = y * GetBytesPerLine() + x*GetPixelSize();

			char *pix_base = data_.data() + offset_target;


			int src_y = this->height_ - y - 1;
			int line_cnt = Align4(src_pix_size*width_);
			int offset_src = 54 + src_y * line_cnt + x*src_pix_size;

			char *src_pix_base = buff.data() + offset_src;

			// printf("(%d, %d) %d(%010d) --> %d(%010d)\n", x, y, offset_src, src_pix_base, offset_target, pix_base);

			pix_base[0] = src_pix_base[0];
			pix_base[1] = src_pix_base[1];
			pix_base[2] = src_pix_base[2];
			pix_base[3] = 0;
		}
	}
}

char* Img::GetData() { return data_.data(); }
int Img::GetSize() { return data_.size(); }
int Img::GetPixelSize() { return 4; };
int Img::GetBytesPerLine() { return GetPixelSize() * width_; };
int Img::GetWidth(){ return width_; }
int Img::GetHeight(){ return height_; }
std::string Img::GetName() { return name_; }


char* GetPixel(Img& img, int x, int y) {
	return img.GetData() + y*img.GetBytesPerLine() + x*img.GetPixelSize();
}

Img Rotate(Img &src, double thelta, GL filtering) {
	Img ret(src);

	int write_rotateX = src.GetWidth()/2;
	int write_rotateY = src.GetHeight()/2;
	int rotateX = write_rotateX;
	int rotateY = write_rotateY;
	
	for (int y = 0; y < src.GetHeight(); ++ y) {
        for (int x = 0; x < src.GetWidth(); ++ x) {
            double d_original_img_y = (x - write_rotateX) * sin(thelta) + (y - write_rotateY) * cos(thelta) + rotateY;
            double d_original_img_x = (x - write_rotateX) * cos(thelta) - (y - write_rotateY) * sin(thelta) + rotateX;    

            if( d_original_img_y < 0 || d_original_img_y >= src.GetHeight()-1 ||
				d_original_img_x < 0 || d_original_img_x >= src.GetWidth()-1 ) {
                continue;
            }
			int Ax = d_original_img_x;
			int Ay = d_original_img_y;
			double distance_to_a_X = d_original_img_x - Ax;
			double distance_to_a_Y = d_original_img_y - Ay;
			/*
			A(x,y)  E
		   -+-------+----+B(x+1,y)
		   ^|<- a ->|    |
		   b|       |    |
		   v|       |P   |
		   -|-------+----|
			|       |F   |
			+-------+----+D(x+1,y+1)
			C(x, y+1)
			*/
			uint8_t *A = reinterpret_cast<uint8_t*> (GetPixel(src, Ax  , Ay));
			uint8_t *B = reinterpret_cast<uint8_t*> (GetPixel(src, Ax+1, Ay));
			uint8_t *C = reinterpret_cast<uint8_t*> (GetPixel(src, Ax  , Ay+1));
			uint8_t *D = reinterpret_cast<uint8_t*> (GetPixel(src, Ax+1, Ay+1));
			uint8_t *target = reinterpret_cast<uint8_t*> (GetPixel(ret, x, y));

			auto do_merge = [] (uint8_t A, uint8_t B, uint8_t C, uint8_t D, double a, double b)->uint8_t {
				double E = (1-a) * A + a*D;
				double F = (1-a) * B + a*C;
				double P = (1-b) * E + b*F;
				return P;
			};
			if (filtering == kLinear) {
				for (int i=0; i<3; ++i) { // BGR888
					target[i] = do_merge(A[i], B[i], C[i], D[i], distance_to_a_X, distance_to_a_Y);
				}
			} else if (filtering == kNearest) {
				Point nearest;
				nearest.x = Ax + (distance_to_a_X>0.5);
				nearest.y = Ay + (distance_to_a_Y>0.5);

				int *base = reinterpret_cast<int*> (GetPixel(src, nearest.x, nearest.y));
				*reinterpret_cast<int*>(target) = *base;
			}
			
        }
    }
	return ret;
}


Img Scale(Img &src, double rate, GL filtering) {
	Img ret(src.GetWidth() * rate, src.GetHeight() * rate, src.GetName() + std::to_string(rate));
	for (int y=0; y<ret.GetWidth(); ++y) {
		for (int x=0; x<ret.GetHeight(); ++x) {
			double src_x = x / rate;
			double src_y = y / rate;

			int Ax = src_x;
			int Ay = src_y;
			double distance_to_a_X = src_x - Ax;
			double distance_to_a_Y = src_y - Ay;

			int Bx = Ax+1;
			int By = Ay;
			int Cx = Ax;
			int Cy = Ay+1;
			int Dx = Ax+1;
			int Dy = Ay+1;

			if (Dx>=src.GetWidth()) {
				Dx = Bx = Ax;
			}
			if (Dy>=src.GetHeight()) {
				Dy = Cy = Ay;
			}

			// printf("(%d, %d), (%d, %d), (%d, %d), (%d, %d) --> (%d, %d) [%d*%d]\n",
			// 	Ax, Ay, Bx, By, Cx, Cy, Dx, Dy, x, y, ret.GetWidth(), ret.GetHeight());

			uint8_t *A = reinterpret_cast<uint8_t*> (GetPixel(src, Ax, Ay));
			uint8_t *B = reinterpret_cast<uint8_t*> (GetPixel(src, Bx, By));
			uint8_t *C = reinterpret_cast<uint8_t*> (GetPixel(src, Cx, Cy));
			uint8_t *D = reinterpret_cast<uint8_t*> (GetPixel(src, Dx, Dy));
			uint8_t *target = reinterpret_cast<uint8_t*> (GetPixel(ret, x, y));
			auto do_merge = [] (uint8_t A, uint8_t B, uint8_t C, uint8_t D, double a, double b)->uint8_t {
				double E = (1-a) * A + a*D;
				double F = (1-a) * B + a*C;
				double P = (1-b) * E + b*F;
				return P;
			};

			if (filtering == kLinear) {
				for (int i=0; i<3; i++) { // BGR888
					target[i] = do_merge(A[i], B[i], C[i], D[i], distance_to_a_X, distance_to_a_Y);
				}
			} else if (filtering == kNearest) {
				Point nearest;
				nearest.x = Ax + (distance_to_a_X>0.5);
				nearest.y = Ay + (distance_to_a_Y>0.5);
				// printf("(%d, %d) --> (%d,%d)\n", x, y, nearest.x, nearest.y);
				int *base = reinterpret_cast<int*> (GetPixel(src, nearest.x, nearest.y));
				*reinterpret_cast<int*>(target) = *base;
			}
			
		}
	}
	return ret;
}
// #define DEBUG printf("%d\n", __LINE__);
#define DEBUG 
bool SaveImg(Img img, std::string file_name) {
	std::vector<uint8_t> buff = {0x42, 0x4d, 0x66, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	*reinterpret_cast<int*> (&buff[0x12]) = img.GetWidth();
	*reinterpret_cast<int*> (&buff[0x16]) = img.GetHeight();
	int width = img.GetWidth();
	int height = img.GetHeight();
	for (int y=height-1; y>=0; --y) {
		for (int x=0; x<width; ++x) {
			uint8_t* pix = reinterpret_cast<uint8_t*> (GetPixel(img, x, y));
			buff.push_back(pix[0]);
			buff.push_back(pix[1]);
			buff.push_back(pix[2]);
		}
		if (width%4 != 0) {
			for (int i=0; i < 4-(3*width)%4; i++) {
				buff.push_back(0);
			}
		}
	}
	std::ofstream out(file_name, std::ios::out|std::ios::binary);
	if (!out.is_open()) {
		std::cout << "open failed" << std::endl;
		return false;
	}
	out.write(reinterpret_cast<char*> (buff.data()), buff.size());
	std::cout << "succ" << std::endl;
}

/*
int main(){
	Img img("music/music_icon.bmp");
	// for (double rate = 0.1; rate<=2.0; rate+=0.1) {
	// 	auto curr = Scale(img, rate);
	// 	DEBUG
	// 	std::cout << curr.GetName() << std::endl;
	// 	std::string file_name = curr.GetName()+".bmp";
	// 	DEBUG
	// 	SaveImg(curr, file_name);
	// 	DEBUG
	// }
	for (int angle = 0; angle<360; angle+=3){
		std::to_string(angle);
		auto curr = Rotate(img, angle/360.0 * 3.14*2, kNearest);
		SaveImg(curr, "1/" + std::to_string(angle)+"_.bmp");
	}
	for (int angle = 0; angle<360; angle+=3){
		std::to_string(angle);
		auto curr = Rotate(img, angle/360.0 * 3.14*2, kLinear);
		SaveImg(curr, "2/" + std::to_string(angle)+"_.bmp");
	}
}
*/