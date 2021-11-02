TARGET  := OpenHCL
TITLE	:= Open Hydra Castle Labyrinth
TITLEID := OHCL00001

LIBS = -lSceLibKernel_stub -lstdc++ -lpthread -lSDL2_Mixer -lSDL2 -lflac -lmpg123 \
		-lmikmod -lvorbisfile -lvorbis -logg -lSceDisplay_stub -lSceCtrl_stub \
		-lSceAudio_stub -lSceSysmodule_stub -lSceGxm_stub -lSceCommonDialog_stub \
		-lSceAppMgr_stub -lSceTouch_stub -lSceIofilemgr_stub -lSceHid_stub \
		-lSceMotion_stub -lSceKernelDmacMgr_stub -lSceShaccCg_stub -lSceLibKernel_stub \
		-lSceAppUtil_stub -lScePgf_stub

INCLUDES := ../../include
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(dir))

CXXFLAGS += $(INCLUDE) -std=c++11
CXXFLAGS += -D_VITA -D_SDL2 -D_NO_EXIT -D_NO_LERP -D_NO_FRAMESKIP -D_HAS_FULLSCREEN

SOURCES := $(wildcard ../../source/*.cpp)
SOURCES += $(wildcard ../../source/effects/*cpp)
SOURCES += $(wildcard ../../source/enemies/*cpp)
SOURCES += $(wildcard ../../source/objects/*cpp)
SOURCES += $(wildcard ../../source/weapons/*cpp)
SOURCES += $(wildcard ../SDL2/source/*cpp)

OBJS := $(SOURCES:.cpp=.o)

CC := arm-vita-eabi-gcc
CXX := arm-vita-eabi-g++
STRIP := arm-vita-eabi-strip

all: $(TARGET).vpk

$(TARGET).vpk: eboot.bin param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
	$(TARGET).vpk

eboot.bin: $(TARGET).velf
	vita-make-fself $(TARGET).velf eboot.bin

param.sfo:
	vita-mksfoex -s TITLE_ID="$(TITLEID)" "$(TITLE)" param.sfo

$(TARGET).velf: $(TARGET).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -Wl,-q -o $@

clean:
	rm -f $(TARGET).velf $(TARGET).elf $(TARGET).vpk param.sfo eboot.bin $(OBJS)
	rm -r $(abspath $(OBJ_DIRS))