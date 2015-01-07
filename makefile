# This borrows heavily from many examples on the World Wide Web

EXECUTABLE=GameOfLife

CC=gcc
LD=gcc

CFLAGS=-c -std=c99 -g

UNAME_S=$(shell uname -s)
ifeq ($(UNAME_S),MINGW32_NT-6.1)
ifdef GLUT
  CFLAGS+=-D FREEGLUT_STATIC -Wall
  LDFLAGS=-lfreeglut_static -lopengl32 -lglu32 -lwinmm -lgdi32 -Wl,--subsystem,windows
endif
endif
ifeq ($(UNAME_S),Darwin)
ifdef GLUT
  CFLAGS+=-Wall -I/usr/X11/include
  LDFLAGS=-L/usr/X11/lib -lGL -lGLU -lglut
endif
endif
ifeq ($(UNAME_S),Linux)
  LDFLAGS=-pthread
endif
SOURCES=$(shell ls *.c)                                              
OBJECTS=$(SOURCES:%.c=%.o)

default:
	echo "Always use make with a target."
	echo "all     -- cleans up then makes everything."
	echo "clean   -- removes most non-source files."
	echo "clobber -- removes all non-source files."
	echo $(EXECUTABLE) "-- makes the program."

all: clean $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm *.o
	-rm $(EXECUTABLE)
	-rm $(EXECUTABLE).exe
	-rm -r $(EXECUTABLE).dSYM

clobber: clean
	-rm *~
