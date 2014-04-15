/**
 * @file TetrisSDL.cpp
 * @author Hiroo MATSUMOTO <hiroom2.mail@gmail.com>
 */
#include <TetrisSDL.h>
#include <iostream>
#include <unistd.h>

Sprite TetrisDrawerSDL::loadSprite(const char* file, SDL_Renderer* renderer)
{
  Sprite sprite;
  SDL_Surface *surface;

  sprite.texture = NULL;
  sprite.width = -1;
  sprite.height = -1;

  surface = SDL_LoadBMP(file);
  if (surface == NULL) {
    std::cerr << "<error> SDL_LoadBMP(" << file << ")\n";
    return sprite;
  }
  sprite.width = surface->w;
  sprite.height = surface->h;

  sprite.texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!sprite.texture)
    std::cerr << "<error> SDL_CreateTextureFromSurface(" << file << ")\n";
  SDL_FreeSurface(surface);

  return sprite;
}

TetrisDrawerSDL::TetrisDrawerSDL(Tetris *tetris)
  : TetrisDrawer(tetris)
{
  SDL_CreateWindowAndRenderer(TETRIS_SDL_WIDTH, TETRIS_SDL_HEIGHT,
                              0, &mWindow, &mRenderer);
  mFrameSprite = loadSprite(TETRIS_FRAME_BITMAP, mRenderer);
  mBarSprite = loadSprite(TETRIS_BAR_BITMAP, mRenderer);

  SDL_GetWindowSize(mWindow, &mWindowWidth, &mWindowHeight);
  mBlockWidth = mWindowWidth / 18;
  mBlockHeight = mWindowHeight / 26;

  TTF_Init();
  mFont = TTF_OpenFont(TETRIS_FONT_FILE, 16);
}

TetrisDrawerSDL::~TetrisDrawerSDL()
{
  ;
}

int TetrisDrawerSDL::type2index(BarType type)
{
  switch (type) {
#define CASE(type, index) case type: { return index; }
    CASE(BAR_TYPE_E, 0);
    CASE(BAR_TYPE_I, 1);
    CASE(BAR_TYPE_J, 2);
    CASE(BAR_TYPE_L, 3);
    CASE(BAR_TYPE_O, 4);
    CASE(BAR_TYPE_S, 5);
    CASE(BAR_TYPE_T, 6);
    CASE(BAR_TYPE_Z, 7);
#undef CASE
  default:
    break;
  };
  return -1;
}

void TetrisDrawerSDL::drawFrame(TetrisField *field, int srcRow,
                                int srcCol, int dstRow, int dstCol)
{
#define RECT(r, c, w, h) { (c) * (w), (r) * (h), (w), (h) }
  SDL_Rect srcrect = RECT(srcRow, srcCol, mFrameSprite.width / 3,
                          mFrameSprite.height / 3);
  SDL_Rect dstrect = RECT(dstRow, dstCol, mBlockWidth, mBlockHeight);
#undef RECT
  SDL_RenderCopy(mRenderer, mFrameSprite.texture, &srcrect, &dstrect);
}

void TetrisDrawerSDL::drawFrameTop(TetrisField *field, int dstRow,
                                   int baseCol)
{
  int col = field->getCol();
  drawFrame(field, 0, 0, dstRow, baseCol);
  for (int c = 1; c <= col; ++c)
    drawFrame(field, 0, 1, dstRow, c + baseCol);
  drawFrame(field, 0, 2, dstRow, col + 1 + baseCol);
}

void TetrisDrawerSDL::drawFrameButtom(TetrisField *field, int dstRow,
                                      int baseCol)
{
  int col = field->getCol();
  drawFrame(field, 2, 0, dstRow, baseCol);
  for (int c = 1; c <= col; ++c)
    drawFrame(field, 2, 1, dstRow, c + baseCol);
  drawFrame(field, 2, 2, dstRow, col + 1 + baseCol);
}

void TetrisDrawerSDL::drawFrameInner(TetrisField *field, int dstRow,
                                     int baseCol)
{
  drawFrame(field, 1, 0, dstRow, baseCol);
  drawFrame(field, 1, 2, dstRow, field->getCol() + 1 + baseCol);
}

void TetrisDrawerSDL::drawFrame(TetrisField *field, int baseCol)
{
  int row = field->getRow();
  drawFrameTop(field, 0, baseCol);
  for (int r = 1; r <= row; ++r)
    drawFrameInner(field, r, baseCol);
  drawFrameButtom(field, row + 1, baseCol);
}

void TetrisDrawerSDL::drawBar(int row, int col, BarType type)
{
  SDL_Rect srcrect = { type2index(type) * mBarSprite.height, 0,
                       mBarSprite.height, mBarSprite.height };
  SDL_Rect dstrect = { col * mBlockWidth, row * mBlockHeight,
                       mBlockWidth, mBlockHeight };
  SDL_RenderCopy(mRenderer, mBarSprite.texture, &srcrect, &dstrect);
}

void TetrisDrawerSDL::drawBar(const TetrisBar *bar, int rot,
                              int row, int col)
{
  BarType type = bar->getType();
  int indexSize = bar->getIndexSize();
  for (int pos = 0; pos < indexSize; ++pos) {
    TetrisIndex index = bar->getIndex(pos, rot);
    drawBar(row + index.r + 1, col + index.c + 1, type);
  }
}

void TetrisDrawerSDL::drawBar(TetrisField *field, int baseCol)
{
  const TetrisBar *bar = field->getBar();
  TetrisIndex barIndex = field->getBarIndex();
  int rot = field->getBarRot();
  drawBar(bar, rot, barIndex.r, barIndex.c + baseCol);
}

void TetrisDrawerSDL::drawField(TetrisField *field, int baseCol)
{
  int row = field->getRow();
  int col = field->getCol();
  for (int r = 0; r < row; ++r)
    for (int c = 0; c < col; ++c)
      drawBar(r + 1, baseCol + c + 1, field->getGrid(r, c));
}

void TetrisDrawerSDL::drawNextBar(TetrisField *field, int baseCol)
{
  const TetrisBar *nextBar = field->getNextBar();
  int nextRot = field->getNextBarRot();
  drawBar(nextBar, nextRot, 2, TETRIS_FIELD_COL + 3 + baseCol);
}

void TetrisDrawerSDL::drawChar(Uint16 ch, int row, int col)
{
  SDL_Color color = { 255, 255, 0 };
  SDL_Surface *surface = TTF_RenderGlyph_Solid(mFont, ch, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
  SDL_Rect srcrect = { 0, 0, surface->w, surface->h };
  SDL_Rect dstrect = { col * mBlockWidth, row * mBlockHeight,
                       mBlockWidth, mBlockHeight };
  SDL_RenderCopy(mRenderer, texture, &srcrect, &dstrect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void TetrisDrawerSDL::drawString(const wchar_t *str, int row, int col)
{
  while (*str != '\0')
    drawChar(*str++, row, col++);
}

void TetrisDrawerSDL::drawValue(int value, int row, int col)
{
  static const char number[] = "0123456789";
  int digit = 0;
  unsigned remain = value;

  while (remain >= 10) {
    remain /= 10;
    digit++;
  }

  while (digit >= 1) {
    drawChar(number[value % 10], row, col + digit);
    value /= 10;
    digit--;
  }

  drawChar(number[value % 10], row, col);
}

#define TETRIS_DRAW(field, name, r, c)                               \
  do {                                                               \
    static const wchar_t prefix[] = L"" #name ": ";                  \
    static const size_t size = sizeof(prefix) / sizeof(*prefix) - 1; \
    drawString(prefix, r, c);                                        \
    drawValue(field->get##name(), r, c + size);                      \
  } while (false)

void TetrisDrawerSDL::drawScore(TetrisField *field, int baseCol)
{
  TETRIS_DRAW(field, Score, field->getRow() + 3, baseCol);
}

#undef TETRIS_DRAW

void TetrisDrawerSDL::gameover()
{
  drawString(L"Game Over", mTetris->getField()->getRow() / 2, 4);
  update();
  SDL_Delay(3000);
}

#define DIRECT_SIZE (0.05f)

enum DirectType {
  DIRECT_TYPE_EMPTY,
  DIRECT_TYPE_UP,
  DIRECT_TYPE_DOWN,
  DIRECT_TYPE_LEFT,
  DIRECT_TYPE_RIGHT,
};

DirectType getDirectType(float dx, float dy)
{
  if (dx >= DIRECT_SIZE)
    return DIRECT_TYPE_RIGHT;
  if (dx < -DIRECT_SIZE)
    return DIRECT_TYPE_LEFT;
  if (dy >= DIRECT_SIZE)
    return DIRECT_TYPE_DOWN;
  if (dy < -DIRECT_SIZE)
    return DIRECT_TYPE_UP;
  return DIRECT_TYPE_EMPTY;
}

#ifdef ANDROID
static int threadFunction(void *data)
{
  InputerThreadData *threadData = (InputerThreadData *) data;
  DirectType directType;
  SDL_Event event;
  float dx = 0;
  float dy = 0;

  while (!threadData->stop) {
    if (threadData->interrupt)
      break;
    if (SDL_WaitEvent(&event) < 0)
      continue;
    if (event.type == SDL_QUIT) {
      *threadData->inputType = INPUT_TYPE_QUIT;
      continue;
    }

    if (event.type != SDL_FINGERMOTION)
      continue;

    dx += event.tfinger.dx;
    dy += event.tfinger.dy;

    directType = getDirectType(dx, dy);
    if (directType == DIRECT_TYPE_EMPTY)
      continue;

    dx = 0;
    dy = 0;

    switch (directType) {
#define CASE(key, type) \
      case key: { *threadData->inputType = type; break; }
      CASE(DIRECT_TYPE_UP, INPUT_TYPE_ROT_LEFT);
      CASE(DIRECT_TYPE_DOWN, INPUT_TYPE_DOWN);
      CASE(DIRECT_TYPE_RIGHT, INPUT_TYPE_RIGHT);
      CASE(DIRECT_TYPE_LEFT, INPUT_TYPE_LEFT);
#undef CASE
    default: { break; }
    }
  }

  return 0;
}

TetrisInputerSDL::TetrisInputerSDL(Tetris *tetris)
  : TetrisInputer(tetris)
{
  mInputType = INPUT_TYPE_EMPTY;
  mThreadData.inputType = &mInputType;
  mThread = SDL_CreateThread(threadFunction, INPUTER_THREAD_NAME,
                             &mThreadData);
}

TetrisInputerSDL::~TetrisInputerSDL()
{
  mThreadData.stop = true;
}

InputType TetrisInputerSDL::input()
{
  if (mInputType == INPUT_TYPE_EMPTY)
    return INPUT_TYPE_EMPTY;

  InputType ret = mInputType;
  mInputType = INPUT_TYPE_EMPTY;
  return ret;
}
#else
TetrisInputerSDL::TetrisInputerSDL(Tetris *tetris)
  : TetrisInputer(tetris)
{

}

TetrisInputerSDL::~TetrisInputerSDL()
{

}

InputType TetrisInputerSDL::input()
{
  SDL_Event event;
  if (SDL_PollEvent(&event) < 0)
    return INPUT_TYPE_EMPTY;

  if (event.type == SDL_QUIT)
    return INPUT_TYPE_QUIT;

  if (event.type != SDL_KEYDOWN)
    return INPUT_TYPE_EMPTY;

  switch (event.key.keysym.sym) {
#define CASE(key, type) \
    case key: { return type; }
    CASE(SDLK_UP, INPUT_TYPE_UP);
    CASE(SDLK_DOWN, INPUT_TYPE_DOWN);
    CASE(SDLK_RIGHT, INPUT_TYPE_RIGHT);
    CASE(SDLK_LEFT, INPUT_TYPE_LEFT);
    CASE(SDLK_z, INPUT_TYPE_ROT_LEFT);
    CASE(SDLK_x, INPUT_TYPE_ROT_RIGHT);
#undef CASE
  default:
    break;
  }

  return INPUT_TYPE_EMPTY;
}
#endif

TetrisSDL::TetrisSDL()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  registerDrawer(mDrawer = new TetrisDrawerSDL(this));
  registerInputer(mInputer = new TetrisInputerSDL(this));
  registerTimer(mTimer = new TetrisTimerPthread(this));
}

TetrisSDL::~TetrisSDL()
{
  SDL_Quit();
  delete mDrawer;
  delete mInputer;
  delete mTimer;
}
