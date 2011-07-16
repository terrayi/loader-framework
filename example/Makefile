#PREFIX = i386-mingw32-
RES = ${PREFIX}windres
ROP = --use-temp-file --define NOPCH
CC = ${PREFIX}gcc
COP = -DHAVE_W32API_H -Wl,--subsystem,windows
COP+= -mwindows -lwinmm -ladvapi32 -lstdc++
STRIP = ${PREFIX}strip -s
TARGET = ExampleLoader.exe
SRC = \
	../src/loader_splash.cpp \
	../src/loader_window.cpp \
	../src/loader_engine.cpp \
	../src/loader_application.cpp \
	../src/loader_utility.cpp \
	myloader.cpp \
	rc.o

all: $(SRC)
	$(CC) -Wall -o $(TARGET) $(SRC) $(COP)
	$(STRIP) $(TARGET)

rc.o: loader.rc
	$(RES) $(ROP) -iloader.rc -o $@

