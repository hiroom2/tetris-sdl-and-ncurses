CXXFLAGS = -Wall -I.
UNAME    = $(shell uname -s)

SDL_SRC = Tetris.cpp TetrisSDL.cpp SDL.cpp
ifeq ($(UNAME), Darwin)
	SDL_TTF_CXXFLAGS = -I/Library/Frameworks/SDL2_ttf.framework/Headers/
  SDL_LIB = -lpthread -framework SDL2 -framework SDL2_ttf
else
  SDL_LIB = -lpthread -lSDL2 -lSDL2_ttf
endif

NCURSES_SRC = Tetris.cpp TetrisNcurses.cpp ncurses.cpp
NCURSES_LIB = -lpthread -lncurses

all: clean sdl ncurses

sdl:
	$(CXX) $(CXXFLAGS) $(SDL_TTF_CXXFLAGS) `sdl2-config --cflags` -o sdl $(SDL_SRC) $(SDL_LIB)

ncurses:
	$(CXX) $(CXXFLAGS) -o ncurses $(NCURSES_SRC) $(NCURSES_LIB)

clean:
	rm -rf sdl ncurses *.dSYM
