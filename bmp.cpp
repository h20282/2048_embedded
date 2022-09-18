#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include "lcd.h"
#include "img.h"

extern int *plcd;

//#define printf("fun:%s, line:%d\n", __func__, __LINE__);

void bmp_display(const char *bmpfile, int x0, int y0)
{
	printf("read file: %s\n", bmpfile);
	int fd;

	fd = open(bmpfile, O_RDONLY);
	if (fd == -1)
	{
	}

	int width, height; //bmp图片的分辨率，宽 ，高
	short depth; //bmp图片的色深

	lseek(fd, 0x12, SEEK_SET);
	read(fd, &width, 4);


	lseek(fd, 0x16, SEEK_SET);
	read(fd, &height, 4);

	printf("%d X %d\n", width, height);

	lseek(fd, 0x1c, SEEK_SET);
	read(fd, &depth, 2);
	if ( !(depth == 24 || depth == 32) )
	{
		printf("Sorry, Not supported\n GoodBye\n");
		return ;
	}

	int valid_line_bytes; //一行的有效数据字节数
	int laizi = 0; //一行的“赖子”字节数
	int line_bytes; //一行总字节数
	int total_bytes; // 整个像素数组字节数

	valid_line_bytes = abs(width) * (depth/8);
	if (valid_line_bytes % 4)
	{
		laizi = 4 - valid_line_bytes % 4;
	}
	line_bytes = valid_line_bytes + laizi;
	total_bytes = line_bytes * abs(height);

	unsigned char *pixel = (unsigned char*) malloc(total_bytes);
	
	lseek(fd, 54, SEEK_SET);
	read(fd, pixel, total_bytes);

	//把像素数组中的颜色数据，显示在屏幕上
	int i = 0;
	unsigned char a,r,g,b;
	int color;
	int x, y;
	for (y = 0; y < abs(height); y++)
	{
		for (x = 0; x < abs(width); x++)
		{
			b = pixel[i++];
			g = pixel[i++];
			r = pixel[i++];
			if (depth == 32)
			{
				a = pixel[i++];
			}
			else
			{
				a = 255;
			}
			color = (a << 24) | (r << 16) | (g << 8) |(b);

			draw_point( x0 + x,	y0 + height - 1 - y	, color);
		}
		i += laizi; //跳过“赖子”
	}

	free(pixel);
	close(fd);
}
void bmp_display(Img &img, int x0, int y0){
	printf("draw `%s` to (%d,%d) [%d*%d]\n", img.GetName().c_str(), x0, y0, img.GetWidth(), img.GetHeight());
	// for (int y=0; y<img.GetHeight(); ++y) {
	// 	for (int x=0; x<img.GetWidth(); ++x) {
	// 		//printf("(%d,%d)\n", x, y);
	// 		char *pix = img.GetData() + y*img.GetBytesPerLine() + x*img.GetPixelSize();
	// 		char b = pix[0];
	// 		char g = pix[1];
	// 		char r = pix[2];
	// 		char a = 0;
	// 		int color = (a << 24) | (r << 16) | (g << 8) |(b);
	// 		draw_point( x0 + x,	y0 + y, color);
	// 	}
	// }
	for (int y=0; y<img.GetHeight(); ++y) {
		memcpy(plcd + (y0+y)*800 + x0, img.GetData() + y*img.GetBytesPerLine(), img.GetBytesPerLine());
	}
}

