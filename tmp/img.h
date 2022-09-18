#include "Img.h"

namespace{
inline int Align4(int n){
	return ((n+3)>>2 ) << 2;
}
}

Img::Img(const char* file_name) {
	std::ifstream file(file_name, std::ios::in|std::ios::binary);
	auto l = file.tellg();
	file.seekg(0, std::ios::end);
	auto file_size = file.tellg() - l;
	std::vector<char> buff(file_size, 0);

	file.read(buff.data(), file_size);

	this->width_ = *reinterpret_cast<int*>(buff.data() + 0x12);
	this->height_ = *reinterpret_cast<int*>(buff.data() + 0x16);
	uint16_t src_pix_size = *reinterpret_cast<uint16_t*>(buff.data() + 0x1c);

	this->data_.resize(buff.size() - 54);
	for (int y=0; y<height_; ++y) {
		for (int x=0; x<height_; ++x) {
			char *pix_base = data_.data() + y * GetBytesPerLine() + x*GetPixelSize();

			int src_y = this->height_ - y - 1;
			int line_cnt = Align4(src_pix_size*width_);
			char *src_pix_base = buff.data() + src_y * line_cnt + x*src_pix_size;

			pix_base[0] = src_pix_base[0];
			pix_base[1] = src_pix_base[1];
			pix_base[2] = src_pix_base[2];
			pix_base[2] = 0;
		}
	}
}

char* Img::GetData() { return data_.data(); }
int Img::GetPixelSize() { return 4; };
int Img::GetBytesPerLine() { return GetPixelSize() * width_; };
int Img::GetWidth(){ return width_; }
int Img::GetHeight(){ return height_; }


int main(){
	for (int i=0; i<20; i++) {
		std::cout << i << " " << Align4(i) << std::endl;
	}
	Img("main.cpp");
}
