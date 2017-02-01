.PHONY: clean

CC := gcc
CFLAGS := -std=c99 -Wall
INCLUDES := -lglut -lGL -lpthread
game: main.c shape.c shapeList.c
	$(CC) $(CFLAGS) main.c shape.c shapeList.c $(INCLUDES) -o game
clean:
	rm -f game
