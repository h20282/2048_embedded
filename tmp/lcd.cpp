/*
+-------------------------------> x
|
|
|
y
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define DEBUG 1


int fd; // 文件标识
int *plcd;

int _2(int n){
	return n*n;
}

int dis(int x1, int y1, int x2, int y2) {
	return _2(x2-x1) + _2(y2-y1);
}

int lcd_open(){
	// 返回值 -1 失败 0 成功
	if (DEBUG) printf("in lcd_open:\n"); // debug
	fd = open("/dev/fb0", O_RDWR);
	if ( fd == -1 ) {
		printf("open failed\n");
		return -1;
	} else {
		// mmap(plcd, 480*800*4, PROT_WRITE, MAP_SHARED, fd, 0);
		plcd = (int*) mmap(NULL, 
				800*480*4,
				PROT_WRITE,
				MAP_SHARED,
				fd,
				0
			);
		return 0;
	}
}

void lcd_close(){
	if (DEBUG) printf("in lcd_close:\n"); // debug
	munmap(plcd, 480*800*4);
	close(fd);
}

void draw_point(int x, int y, int color){
	// if (DEBUG) printf("in draw_point:\n"); // debug
	if ( x>=0 && x<800 && y>=0 && y<480){
		plcd[y*800+x] = color;
		// *(plcd + y1*800 + x) = color ;
	} else {
		printf("error int lcd.c->draw_point: x or y out range\n");
	}
}

void draw_circle(int x, int y, int r, int color) {
	int i,j;
	for ( i=x-r; i<=x+r; i++ ) {
		for ( j=y-r; j<=y+r; j++ ) {
			if ( dis(i, j, x, y) <= r*r ) {
				draw_point(i, j, color);
			}
		}
	}
}

void clear_screen(int color){
	if (DEBUG) printf("in clear_screen:\n"); // debug
	int x,y;
	for ( y=0; y<480; y++ ) {
		for ( x=0; x<800; x++) {
			draw_point(x ,y, color);
		}
	}
}

void draw_4_leaves(){
	if (DEBUG) printf("in draw_4_leaves:\n"); // debug
	int x,y;
	int r = 240;
	int red = 0xff0000;
	int black = 0x000000;
	for(y=0; y<480; y++){
		for(x=0; x<800; x++){
			int cnt = 0;
			cnt += dis((int)0, (int)240, x, y ) < r*r;
			cnt += dis((int)240, (int)0, x, y ) < r*r;
			cnt += dis((int)480, (int)240, x, y ) < r*r;
			cnt += dis((int)240, (int)480, x, y ) < r*r;
			if ( cnt >= 2 ) {
				draw_point(x, y, red);
			} else {
				draw_point(x, y, black);
			}
		}
	}
}

void draw_rect(int x, int y, int width, int height, int color){
	int i,j;
	for ( j=0; j<height; j++ ) {
		for ( i=0; i<width; i++ ) {
			// printf("in fun drwa_rect line 113: i = %d, j=%d \n", i, j);
			draw_point(x+i, y+j, color);
		}
	}
}
