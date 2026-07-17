program: 
	gcc wireframe.c $(shell pkg-config --libs --cflags raylib) -o wireframe