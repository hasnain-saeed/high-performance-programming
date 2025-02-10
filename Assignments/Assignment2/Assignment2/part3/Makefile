CC = gcc
CFLAGS = -Wall
RM = /bin/rm -f
EXEC = january
SRC = linkedlist.c driver.c
OBJ = $(SRC:.c=.o)
HEADERS = linkedlist.h

${EXEC}: ${OBJ}
	$(CC) $(CFLAGS) -o ${EXEC} ${OBJ}

${OBJ}: ${SRC} ${HEADERS}
	$(CC) $(CFLAGS) -c ${SRC}

clean:
	$(RM) ${EXEC} ${OBJ}
