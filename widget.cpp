#include "widget.h"

#include "bmp.h"

Widget::Widget(const char* file_name, int x, int y, CallBack on_clicked)
		: img_(file_name), x_(x), y_(y), on_clicked_(on_clicked) { }

void Widget::HandleClick() {
    on_clicked_();
}	

bool Widget::Contain(int x, int y) {
    printf("x_ = %d, y_ = %d, GetWidth() = %d, GetHeight(0 = %d\n", x_, y_, GetWidth(), GetHeight());
    return x>=this->x_ && x<=this->x_+GetWidth() &&
           y>=this->y_ && y<=this->y_+GetHeight();	
}

void Widget::Draw() {
    bmp_display(img_, x_, y_);
}

int Widget::GetWidth() {
    return img_.GetWidth();
} 

int Widget::GetHeight() {
    return img_.GetHeight();
} 