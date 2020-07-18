CC = g++

INCLUDE_PATHS = -IC:\MinGW\mingw32\include
LIBRARY_PATHS = -LC:\MinGW\mingw32\lib

CFLAGS := -D_WIN32 -D_SDL2
CFLAGS += -D_DEBUG -D_HAS_FULLSCREEN -D_HAS_VSYNC -D_HAS_KEYBOARD

LIBS = -lm -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer

#show print console
ifeq (,$(findstring _DEBUG, $(CFLAGS)))
	LIBS += -mwindows
endif

INCLUDES = ../../include

OBJS := $(wildcard ../../source/*.cpp)
OBJS += $(wildcard ../../source/effects/*.cpp)
OBJS += $(wildcard ../../source/enemies/*.cpp)
OBJS += $(wildcard ../../source/objects/*.cpp)
OBJS += $(wildcard ../../source/weapons/*.cpp)
OBJS += $(wildcard ../sdl2/source/*.cpp)

TARGET = OpenHCL
#---------------------------------------------

.PHONY: $(TARGET) clean

all: $(TARGET)

info.o:
	windres info.rc info.o
	
$(TARGET): info.o
	$(CC) $(OBJS) info.o $(INCLUDE_PATHS) -I$(INCLUDES) $(LIBRARY_PATHS) $(CFLAGS) $(LIBS) -o $(TARGET)

clean:
	@rm -fr info.o $(TARGET)