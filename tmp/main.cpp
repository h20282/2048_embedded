#include "lcd.h"
#include "Img.h"


int main(){

	ui.AddWidget(new Button("restart", 570, 400, ()[]{
		for (int i=0; i<arr_len; ++i){
			arr[i] = 0;
		}
	}))

	ui.AddWidget(new Button("sort", sort_x, sort_y, ()[]{
		sort(arr, arr+16);
	}))
}
