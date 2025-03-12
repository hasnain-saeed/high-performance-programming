CC = gcc
LD = gcc
CFLAGS = -O3 -march=native -ftree-vectorize -ffast-math -lpthread -Wall -Werror
LDFLAGS = -lm
RM = /bin/rm -f
SOURCE=galsim_pthread
OBJS = ${SOURCE}.o
EXECUTABLE = galsim

$(EXECUTABLE): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE)

${SOURCE}.o: ${SOURCE}.c
	$(CC) $(CFLAGS) -c ${SOURCE}.c

clean:
	$(RM) $(EXECUTABLE) $(OBJS) result.gal
