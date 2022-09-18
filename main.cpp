#include <cstdlib>
#include <cstdio>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>

#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <functional>
#include <thread>

#include "lcd.h"
#include "bmp.h"
#include "widget.h"
#include "Point.h"
#include "Game2048.h"
#include "music_player.h"

std::string music_file_name;
void* PlayMusic(void *p) {
	system( ("madplay " + music_file_name).c_str() );
}
// void PlayMusic(std::string music_file) {
// 	music_file_name = music_file;
// 	pthread_t tid;
// 	int ret = pthread_create(&tid, NULL, PlayMusic,  NULL);
// }
void PlayMusic(std::string music_file) {
	auto f = [=](){ system( ("madplay " + music_file).c_str() ); };
	std::thread(f).detach();
}

bool Contain(int x, int y, int width, int height, int xx, int yy) {
	return xx>=x && xx<=x+width &&
		   yy>=y && yy<=y+height;
}

std::map<int, Img> digit2img;
void DrawNumber(int number, int x, int y) {
	std::vector<int> arr;
	int curr_x = x;
	while(number) {
		arr.push_back(number%10);
		number /= 10;
	}
	while(arr.size() < 6) {
		arr.push_back(0);
	}
	for(auto iter=arr.rbegin(); iter!=arr.rend(); ++iter) {
		bmp_display(digit2img[*iter], curr_x, y);
		curr_x += digit2img[*iter].GetWidth();
	}
}

/* 返回按下和抬起的坐标，失败返回 (-1, -1)
 */

int fd_input = -2;
void GetTsPoint(Point *down, Point *up) {
	*down = {-1, -1};
	*up = {-1, -1};
	if (fd_input < 0) {
		fd_input = open("/dev/input/event0", O_RDONLY);
		if (fd_input == -1) {
			printf("failed to open /dev/input/event0\n");
			return ;
		} else {
			printf("open /dev/input/event0 success!\n");
		}
	}

	struct input_event ev;
	while (1) {	
		int ret = read(fd_input, &ev, sizeof(ev));
		if (ret != sizeof(ev)) {
			printf("no data\n");
			continue;
		}

		//来的是一个 触摸屏的x轴坐标事件
		if (ev.type == EV_ABS && ev.code == ABS_X) {
			// draw_circle(x2, y2, 3, 0xff0000);
			int x = ev.value/(1014/800.0);
			x = x * 800.0/626;
			printf("get a value: x = %d\n", x);
			if (down->x == -1) { down->x = x; }
			up->x = x;
		}

		if (ev.type == EV_ABS && ev.code == ABS_Y) {
			int y = ev.value/(590/480.0);
			y = y * 480.0/388;
			printf("get a value: y = %d\n", y);
			if (down->y == -1) { down->y = y; }
			up->y = y;
		}

		if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0) {
			break;
		}
	}
}

constexpr int kBoxSize = 100;
constexpr int kSpacing = 10;
constexpr int kStartX = 30;
constexpr int kStartY = 25;

std::map<int, Img> number2img;
void Draw(int *arr) {
	for (int y=0; y<kSize; ++y) {
		for (int x=0; x<kSize; ++x) {
			int pic_x = kStartX + x*(kBoxSize+kSpacing);
			int pic_y = kStartY + y*(kBoxSize+kSpacing);
			//printf("%s -> %d, %d\n", file_name.c_str(), pic_x, pic_y);
			bmp_display( number2img[arr[y*kSize+x]], pic_x, pic_y );
		}
	}
}

int main() {
	MusicPlayer music;
	// PlayMusic("music/wu_ren_zhi_dao.mp3");
	lcd_open();
	

	for (auto i : {0,2,4,8,16,32,64,128,256,512,1024,2048} ) {
		number2img[i] = Img( ("img/" + std::to_string(i) + ".bmp").c_str() );
	}
	for (int i=0; i<10; i++) {
		digit2img[i] = Img( ("img/digit/" + std::to_string(i) + ".bmp").c_str() );
	}

	bool moved = true;
	std::stack<std::vector<int>> status;
	status.push(std::vector<int>(kSize*kSize));

	clear_screen(0x00ffffff);
	Img bg("img/bg.bmp"); // 450*450
	bmp_display(bg, kStartX-10, kStartY-10);

	// Img score("img/score.bmp");
	// bmp_display(score, 550, kStartY);
	Img help_img("img/help_img.bmp");

	Game2048 game(
		[](int *arr){ 
			/*PlayMusic("sound/move.mp3");*/ 
			Draw(arr);
		}, 
		[](int x, int y, int value){
			int pic_x = kStartX + x*(kBoxSize+kSpacing);
			int pic_y = kStartY + y*(kBoxSize+kSpacing);
			bmp_display( number2img[0], pic_x, pic_y );
			// std::thread([&](){
				for (auto rate : {1.0, 0.0975, 0.19, 0.2775, 0.36, 0.4375, 0.51, 0.5775, 0.64, 0.6975, 0.75, 0.7975, 0.84, 0.8775, 0.91, 0.9375, 0.96, 0.9775, 0.99, 0.9975} ) {
					Img scaled = Scale(number2img[value], rate);
					int xx = pic_x + (100-scaled.GetWidth()) / 2;
					int yy = pic_y + (100-scaled.GetHeight()) / 2;
					bmp_display(scaled, xx, yy);
					usleep(50*1000/20);
				}
			// }).detach();
		}
	);

	std::vector<Widget> widgets;
	// 144*55
	widgets.push_back(Widget("img/restart_new.bmp", 500, kStartY+35+10, [&](){
			game.ReStart();
		}));

	widgets.push_back(Widget("img/back_new.bmp", 500+154, kStartY+35+10, [&](){
			game.GoBack();
		}));

	widgets.push_back(Widget("img/reorder.bmp", 500, kStartY+35+10+65, [&](){
			game.Reorder();
		}));

	bool daili = false;
	widgets.push_back(Widget("img/score.bmp", 550, kStartY+0, [&](){
			std::thread([&](){
				daili = false;
				usleep(2000*1000);
				daili = true;
				while(daili) {
					bool can_move = false;
					for (auto dir : {kDown, kLeft, kRight, kUp}) {
						if (game.Move(dir)) {
							can_move = true;
							static std::string dirs[] = {"down", "left", "right", "up"};
							std::cout << "go " << dirs[dir] << std::endl;
							break;
						}
					}
					if (!can_move) {
						printf("back");
						game.GoBack();
					}
					usleep(1000*1000);
				}
			}).detach();
		}));

	bool showing_help = false;
	widgets.push_back(Widget("img/help.bmp", 500+154, kStartY+35+10+65, [&](){
			showing_help = !showing_help;
			if (showing_help){
				bmp_display(help_img, 0, 0);
			} else {
				for (int y=0; y<480; ++y) {
					for (int x=0; x<500; ++x) {
						draw_point(x, y, 0x00ffffff);
					}
				}
				bmp_display(bg, kStartX-10, kStartY-10);
				Draw(game.GetCurr().data());
			}
		}));

	for (auto &widget : widgets) {
		widget.Draw();	
	}
	


	while (1) {
		// todo: 添加`game over` 的判定（不再能移动）
		
		if (!showing_help){
			// Draw(game.GetCurr().data());
		}
		
		Point down, up;
		GetTsPoint(&down, &up);
		printf("down: %d,%d up: %d,%d\n", down, up);
		if (down.x < 0 || down.y<0 || up.x<0 || up.y<0) {
			continue;
		}

		Point vec = up-down;
		if (abs(vec.x) + abs(vec.y) > 10) { //滑动事件
			double k = static_cast<double>(vec.y) / vec.x;
			Dir dir;
			if (k>-1 && k<1) {
				if (vec.x>0) { dir = kRight; }
				else { dir = kLeft; }
			} else {
				if (vec.y>0) { dir = kDown; }
				else { dir = kUp; }
			}
			if (Contain(541, 226, 217, 218, down.x, down.y)  &&
				Contain(541, 226, 217, 218, up.x, up.y)) {
				if (dir==kLeft) {
					music.Pre();
				} else if (dir==kRight) {
					music.Next();
				}
			} else {
				daili = false;
				game.Move(dir);
				printf("game.GetScore() = %d\n", game.GetScore());
				
				for (int x=550 + /*score.GetWidth()*/59; x<800; ++x) {
					for (int y=kStartY; y<kStartY+35; ++y) {
						draw_point(x, y, 0xffffffff);
					}
				}
				DrawNumber( game.GetScore(), 550+/*score.GetWidth()*/59, kStartY);
			}
		} else { // 点击事件
			for (auto& widget : widgets) {
				if (widget.Contain(up.x, up.y)) {
					widget.HandleClick();
				}
			}
			if (Contain(541, 226, 217, 218, up.x, up.y)) {
				music.Toggle();
			}
		}
	
	}
}
