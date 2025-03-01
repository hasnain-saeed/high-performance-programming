CC = gcc
LD = gcc
INCLUDES = -I/opt/X11/include
LIBS = -L/opt/X11/lib -lX11 -lm
CFLAGS = -O3 -march=native -ffast-math -Wall -Werror $(INCLUDES)
CFLAGS_DEBUG = -g $(INCLUDES)
CFLAGS_PROFILE = -g -pg $(INCLUDES)
LDFLAGS = $(LIBS)
RM = /bin/rm -f
SOURCE=galsim_opt
OBJS = ${SOURCE}.o graphics/graphics.o
EXECUTABLE = galsim

all: $(EXECUTABLE)
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(EXECUTABLE)_debug
profile: CFLAGS = $(CFLAGS_PROFILE)
profile: $(EXECUTABLE)_profile

$(EXECUTABLE): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE)

$(EXECUTABLE)_debug: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE)_debug

$(EXECUTABLE)_profile: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -pg -o $(EXECUTABLE)_profile

${SOURCE}.o: ${SOURCE}.c
	$(CC) $(CFLAGS) -c ${SOURCE}.c

graphics/graphics.o: graphics/graphics.c
	$(CC) $(CFLAGS) -c graphics/graphics.c -o graphics/graphics.o

clean:
	$(RM) $(EXECUTABLE) $(EXECUTABLE)_debug $(EXECUTABLE)_profile $(OBJS)
