#TGB_INI_DIR = /opt/QtPalmtop/share/tgbdual

INCLUDES += `sdl-config --cflags` 
#INCLUDES += `sdl-config --cflags` -Dmain=SDL_main -DQWS -D_REENTRANT
LIBS += `sdl-config --libs` -lz
CFLAGS = -O3 -fPIC
CXXFLAGS = -O3 -fPIC -std=c++11

ifdef TGB_INI_DIR
CXXFLAGS += -DTGB_INI_DIR=\"$(TGB_INI_DIR)\" -std=c++11
endif

SOURCES=$(wildcard *.cpp)
CSOURCES=$(wildcard *.c)
OBJS = $(SOURCES:.cpp=.o)
OBJS += $(CSOURCES:.c=.o)

EXT_CSOURCES = $(wildcard ../gb_core/*.c)
EXT_CSOURCES += $(wildcard ../goomba/*.c)
EXT_OBJS = $(EXT_CSOURCES:.c=.o)
EXT_OBJS += $(wildcard ../gb_core/*.o)
all: tgbdual

tgbdual: $(OBJS) $(EXT_OBJS)
	$(CXX) -o tgbdual $(OBJS) $(EXT_OBJS) $(LIBS) $(CXXFLAGS) $(INCLUDES)

sdlmain.o: dialogs.h

%.o:%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

%.o:%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $<

clean:
	rm -f $(OBJS) tgbdual


