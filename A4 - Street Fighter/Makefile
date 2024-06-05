CC=gcc
CFLAGS=-Wall
LIBS=$(shell pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5 --libs --cflags)

a4: a4.c
	$(CC) $(CFLAGS) -o a4 a4.c $(LIBS)

clean:
	rm -f *.o a4
