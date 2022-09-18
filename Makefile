2048.out: *.cpp
	arm-linux-g++	*.cpp	 -o	 2048.out	-std=c++1y -lpthread
