/**
 * @file TetrisSDL.h
 * @author Hiroo MATSUMOTO <hiroom2.mail@gmail.com>
 */
#ifndef __TETRISSDL_H
#define __TETRISSDL_H

#include <Tetris.h>
#include <SDL_ttf.h>

#ifdef ANDROID

#include <SDL.h>
enum {
  TETRIS_SDL_WIDTH = 0,
  TETRIS_SDL_HEIGHT = 0,
};

#define TETRIS_FRAME_BITMAP "Frame.bmp"
#define TETRIS_BAR_BITMAP   "Bar.bmp"
#define TETRIS_FONT_FILE    "ipaexg.ttf"

#else

#include <SDL2/SDL.h>
enum {
  TETRIS_SDL_WIDTH = 400,
  TETRIS_SDL_HEIGHT = 480,
};

#define TETRIS_FRAME_BITMAP "../../assets/Frame.bmp"
#define TETRIS_BAR_BITMAP   "../../assets/Bar.bmp"
#define TETRIS_FONT_FILE    "../../assets/ipaexg.ttf"

#endif

class Sprite {
 public:
  SDL_Texture *texture;
  Uint16 width;
  Uint16 height;
};

class TetrisDrawerSDL : public TetrisDrawer {
 private:
  SDL_Window *mWindow;
  SDL_Renderer *mRenderer;
  Sprite mBarSprite;
  Sprite mFrameSprite;
  TTF_Font *mFont;

  int mWindowWidth;
  int mWindowHeight;
  int mBlockWidth;
  int mBlockHeight;

  Sprite loadSprite(const char* file, SDL_Renderer* renderer);
  int type2index(BarType type);
  void drawFrame(TetrisField *field, int srcRow, int srcCol,
                 int dstRow, int dstCol);
  void drawFrameTop(TetrisField *field, int dstRow, int baseCol);
  void drawFrameButtom(TetrisField *field, int dstRow, int baseCol);
  void drawFrameInner(TetrisField *field, int dstRow, int baseCol);
  void drawBar(int row, int col, BarType type);
  void drawBar(const TetrisBar *bar, int rot, int row, int col);
  void drawChar(Uint16 ch, int row, int col);
  void drawValue(int value, int row, int col);
  void drawString(const wchar_t *str, int row, int col);

 protected:
  void drawFrame(TetrisField * field, int baseCol);
  void drawField(TetrisField *field, int baseCol);
  void drawBar(TetrisField *field, int baseCol);
  void drawScore(TetrisField *field, int baseCol);
  void drawNextBar(TetrisField *field, int baseCol);
  void erase() { SDL_RenderClear(mRenderer); }
  void update() { SDL_RenderPresent(mRenderer); }

 public:
  TetrisDrawerSDL(Tetris *tetris);
  ~TetrisDrawerSDL();
  void gameover();
};

#ifdef ANDROID
struct InputerThreadData {
public:
  InputType *inputType;
  bool stop;
  bool interrupt;

  InputerThreadData() : stop(false), interrupt(false) {}
};

#define INPUTER_THREAD_TIMEOUT (100)
#define INPUTER_THREAD_NAME "InputerThread"
#endif

class TetrisInputerSDL : public TetrisInputer {
#ifdef ANDROID
 private:
  SDL_Thread *mThread;
  InputType mInputType;
  InputerThreadData mThreadData;
#endif

 public:
  TetrisInputerSDL(Tetris *tetris);
  ~TetrisInputerSDL();
  InputType input();
};

class TetrisSDL : public Tetris {
 private:
  TetrisDrawerSDL *mDrawer;
  TetrisInputerSDL *mInputer;
  TetrisTimerPthread *mTimer;

 public:
  TetrisSDL();
  ~TetrisSDL();
};

#endif /* __TETRISSDL_H */
