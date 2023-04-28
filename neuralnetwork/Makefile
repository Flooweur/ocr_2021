# Makefile

CC=gcc -fsanitize=address
CPPFLAGS= -MMD -D_XOPEN_SOURCE=500
CFLAGS= -Wall -Wextra -std=c99 -O2 
LDFLAGS=
LDLIBS= -lm

SRC= neuralnetwork.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}

all: neuralnetwork

-include ${DEP}

neuralnetwork: ${OBJ}

clean:
	rm -f ${OBJ} ${DEP} neuralnetwork

# END
