#ifndef __LCD_H__
#define __LCD_H__

int lcd_open();  // 返回值 -1 失败 0 成功
void lcd_close();
void draw_point(int x, int y, int color);
void draw_circle(int x, int y, int r, int color);
void clear_screen(int color);
void draw_4_leaves();
void draw_rect(int x, int y, int width, int height, int color);

#endif