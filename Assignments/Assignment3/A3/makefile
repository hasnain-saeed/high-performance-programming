CC = gcc
LD = gcc
LIBS = -lm
CFLAGS = -O3 -march=native -ffast-math -Wall -Werror
CFLAGS_DEBUG = -g
CFLAGS_PROFILE = -g -pg
LDFLAGS = $(LIBS)
RM = /bin/rm -f
SOURCE=galsim_opt
OBJS = ${SOURCE}.o
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

clean:
	$(RM) $(EXECUTABLE) $(EXECUTABLE)_debug $(EXECUTABLE)_profile $(OBJS)
