all:
	g++ -Wall -g -I/usr/X11R6/include -c glm.c
	g++ -Wall -g -I/usr/X11R6/include -c lofvlib.c
	g++ -Wall -g -I/usr/X11R6/include -c player.cpp
	g++ -Wall -g -I/usr/X11R6/include -L/usr/X11R6/lib -lglut -lGLU -lGL -lXt -lXi -lXmu -lXext -lX11 -lm  -o engine3d engine3d.c glm.o lofvlib.o player.o

clean:
	rm -f *.o engine3d
