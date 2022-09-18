void get_ts_point(Point &down, Point &up) {
	down = {};
	up = {};
	int fd = open("/dev/input/event0", O_RDONLY);
	if (fd == -1) {
		printf("failed to open /dev/input/event0\n");
		return ;
	} else {
		printf("open /dev/input/event0 success!\n");
	}

	struct input_event ev;
	while (1)
	{	
		int ret = read(fd, &ev, sizeof(ev));
		if (ret != sizeof(ev))
		{
			printf("no data\n");
			continue;
		}

		//来的是一个 触摸屏的x轴坐标事件
		if (ev.type == EV_ABS && ev.code == ABS_X)
		{
			// draw_circle(x2, y2, 3, 0xff0000);
			int x = ev.value/(1014/800.0);
			printf("get a value: x = %d\n", x);
			if (down.x == -1) { down.x = x; }
			up.x = x;
		}

		if (ev.type == EV_ABS && ev.code == ABS_Y)
		{
			int y = ev.value/(590/480.0);
			printf("get a value: y = %d\n", y);
			if (down.y == -1) { down.y = y; }
			up.y = y;
		}

		if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0) {
			break;
		}
	}
}


