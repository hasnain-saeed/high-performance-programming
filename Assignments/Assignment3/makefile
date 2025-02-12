CC = gcc
LD = gcc
INCLUDES = -I/opt/X11/include
LIBS = -L/opt/X11/lib -lX11 -lm
CFLAGS = -O3 -Wall -Werror $(INCLUDES)
CFLAGS_DEBUG = -g -Wall -Werror -O0 $(INCLUDES)
LDFLAGS = $(LIBS)
RM = /bin/rm -f
OBJS = galsim.o graphics/graphics.o
EXECUTABLE = galsim
EXECUTABLE_DEBUG = galsim_debug

all: $(EXECUTABLE)

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(EXECUTABLE_DEBUG)

$(EXECUTABLE): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE)

$(EXECUTABLE_DEBUG): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE_DEBUG)

galsim.o: galsim.c
	$(CC) $(CFLAGS) -c galsim.c

graphics/graphics.o: graphics/graphics.c
	$(CC) $(CFLAGS) -c graphics/graphics.c -o graphics/graphics.o

clean:
	$(RM) $(EXECUTABLE) $(EXECUTABLE_DEBUG) $(OBJS)
