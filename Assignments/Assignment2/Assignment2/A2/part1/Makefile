CC = gcc
CFLAGS = -Wall
RM = /bin/rm -f
EXEC = triang
SRC = pascal.c
OBJ = $(SRC:.c=.o)
HEADERS = pascal.h

${EXEC}: ${OBJ}
	$(CC) $(CFLAGS) -o ${EXEC} ${OBJ}

${OBJ}: ${SRC} ${HEADERS}
	$(CC) $(CFLAGS) -c ${SRC}

clean:
	$(RM) ${EXEC} ${OBJ}
