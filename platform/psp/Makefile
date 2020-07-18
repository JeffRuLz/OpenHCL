INCDIR = ../../include
LIBDIR =

LIBS = -lpspgum -lpspgu -lpsppower -lpspaudiolib -lpspaudio -lvorbisidec -logg -lpspreg -lstdc++ -lm

CFLAGS = -O2 -G0 -Wall -D_PSP -Dnullptr=0
CFLAGS += -D_NO_FRAMESKIP -D_NO_LERP -D_HAS_FULLSCREEN

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) 
LDFLAGS = 

OBJS := $(wildcard ../../source/*.cpp)
OBJS += $(wildcard ../../source/effects/*.cpp)
OBJS += $(wildcard ../../source/enemies/*.cpp)
OBJS += $(wildcard ../../source/objects/*.cpp)
OBJS += $(wildcard ../../source/weapons/*.cpp)
OBJS += $(wildcard source/*.cpp)

TARGET = OpenHCL

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = OpenHCL
PSP_EBOOT_ICON = banner/ICON0.PNG
PSP_EBOOT_PIC1 = banner/PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

clean:
	-$(RM) -f $(FINAL_TARGET) $(EXTRA_CLEAN) $(PSP_EBOOT_SFO) $(PSP_EBOOT) $(EXTRA_TARGETS)