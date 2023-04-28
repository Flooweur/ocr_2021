CC = gcc
CFLAGS = -g -Wall -Werror -Wextra -std=c99 -O1 -lm -ldl `pkg-config --cflags sdl` -lSDL -lpthread -lSDL_image -lSDL_gfx
LDFLAGS = -g
LDLIBS = `pkg-config --libs sdl`


SRC = neuNetwork.c testexport.c griddetection.c pixel_operations.c colors.c main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main


main: ${OBJ}
	${CC} $^ -o $@ ${CFLAGS} ${LDLIBS}

.PHONY: clean

clean: 
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} *.o
	${RM} *.d
	${RM} *.bmp
	${RM} a.out
	${RM} main
	${RM} gridexport
	${RM} gridToSolve

-include ${DEP}
