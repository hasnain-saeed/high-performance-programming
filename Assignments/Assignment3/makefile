CC = gcc
LD = gcc
CFLAGS = -O3 -Wall -Werror
CFLAGS_DEBUG = -g -Wall -Werror -O0
LDFLAGS = -lm
RM = /bin/rm -f
OBJS = galsim.o
EXECUTABLE = galsim
EXECUTABLE_DEBUG = galsim_debug

all: $(EXECUTABLE)

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(EXECUTABLE_DEBUG)

$(EXECUTABLE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(EXECUTABLE)

$(EXECUTABLE_DEBUG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(EXECUTABLE_DEBUG)

galsim.o: galsim.c
	$(CC) $(CFLAGS) -c galsim.c

clean:
	$(RM) $(EXECUTABLE) $(EXECUTABLE_DEBUG) $(OBJS)
