/**
 * @file Tetris.cpp
 * @author Hiroo MATSUMOTO <hiroom2.mail@gmail.com>
 */
#include <Tetris.h>

TetrisBar::TetrisBar(BarType type, const char *str,
                     TetrisIndex rotStart, int rotSize)
  : mType(type), mRotSize(rotSize)
{
  mIndexSize = countNotEmptyGrid(type, str);
  mIndex = new TetrisIndex* [mRotSize];
  for (int rot = 0; rot < mRotSize; ++rot) {
    mIndex[rot] = new TetrisIndex[mIndexSize];
    for (int idx = 0; idx < mIndexSize; ++idx)
      mIndex[rot][idx] = new TetrisIndex();
  }

  int idx = 0;
  for (int r = 0; r < TETRIS_BAR_ROW; ++r)
    for (int c = 0; c < TETRIS_BAR_COL; ++c)
      if (str[rc2point(r, c)] == type) {
        mIndex[0][idx].c = c - rotStart.c;
        mIndex[0][idx].r = r - rotStart.r;
        idx++;
      }

  for (int rot = 1; rot < mRotSize; ++rot)
    for (int idx = 0; idx < mIndexSize; ++idx)
      mIndex[rot][idx] = TetrisIndex::rotate(mIndex[rot - 1][idx]);
}

TetrisField::TetrisField()
  : mRow(TETRIS_FIELD_ROW), mCol(TETRIS_FIELD_COL),
    mScore(0), mLines(0)
{
  clear();
  srand((unsigned) time(NULL));
  /** Ignore some ::rand() */
  for (int i = 0, r = rand(1024); i < r; ++i)
    rand();
  mNextBar = getRandBar();
  mNextBarRot = getRandBarRot(mNextBar);
  setBar();
}

TetrisField::~TetrisField()
{

}

bool TetrisField::checkLocatable(TetrisIndex &next, int rot)
{
  int indexSize = mBar->getIndexSize();
  for (int pos = 0; pos < indexSize; ++pos) {
    TetrisIndex index = mBar->getIndex(pos, rot);
    int c = next.c + index.c;
    int r = next.r + index.r;
    if (c < 0 || c >= mCol || r < 0 || r >= mRow)
      return false;
    if (getGrid(r, c) != BAR_TYPE_E)
      return false;
  }
  return true;
}

bool TetrisField::moveBar(int dx, int dy)
{
  TetrisIndex next = TetrisIndex(mBarIndex.c + dx, mBarIndex.r + dy);
  if (!checkLocatable(next, mBarRot))
    return false;
  mBarIndex.c += dx;
  mBarIndex.r += dy;
  return true;
}

bool TetrisField::rotBar(int dr)
{
  int rotSize = mBar->getRotSize();
  int next = (rotSize + mBarRot + dr) % rotSize;
  if (!checkLocatable(mBarIndex, next))
    return false;
  mBarRot = next;
  return false;
}

bool TetrisField::input(InputType inputType)
{
  bool ret = false;
  // TODO: lock
  switch (inputType) {
#define CASE(type, func) case type: { ret = func(); break;}
    CASE(INPUT_TYPE_UP, moveUpBar);
    CASE(INPUT_TYPE_DOWN, moveDownBar);
    CASE(INPUT_TYPE_LEFT, moveLeftBar);
    CASE(INPUT_TYPE_RIGHT, moveRightBar);
    CASE(INPUT_TYPE_ROT_LEFT, rotLeftBar);
    CASE(INPUT_TYPE_ROT_RIGHT, rotRightBar);
#undef CASE
  default:
    {
      ret = false;
      break;
    }
  }
  // TODO: unlock
  return ret;
}

void TetrisField::putBar()
{
  BarType type = mBar->getType();
  int indexSize = mBar->getIndexSize();
  for (int pos = 0; pos < indexSize; ++pos) {
    TetrisIndex index = mBar->getIndex(pos, mBarRot);
    setGrid(mBarIndex.r + index.r, mBarIndex.c + index.c, type);
  }
}

bool TetrisField::checkLine(int row)
{
  for (int col = 0; col < mCol; ++col)
    if (getGrid(row, col) == BAR_TYPE_E)
      return false;
  return true;
}

void TetrisField::deleteLine(int row)
{
  for (int r = row - 1; r >= 0; --r)
    for (int c = 0; c < mCol; ++c)
      setGrid(r + 1, c, getGrid(r, c));
  for (int c = 0; c < mCol; ++c)
    setGrid(0, c, BAR_TYPE_E);
}

void TetrisField::deleteLine()
{
  unsigned lines = 0;
  for (int row = mRow - 1; row >= 0; --row)
    if (checkLine(row)) {
      deleteLine(row++); //TODO: O(n^2) -> O(n)
      lines++;
    }

  if (lines) {
    mScore += lines;
    mLines += lines;
  }
}

void TetrisDrawer::draw(TetrisField *field, int baseCol)
{
  drawFrame(field, baseCol);
  drawField(field, baseCol);
  drawBar(field, baseCol);
  drawScore(field, baseCol);
  drawNextBar(field, baseCol);
}

void TetrisDrawer::draw()
{
  erase();
  draw(mTetris->getField(), 0);
  update();
}

bool TetrisField::timer()
{
  bool ret = true;
  /* TODO: lock */
  if (!moveDownBar()) {
    putBar();
    if (!setBar())
      ret = false;
    else
      deleteLine();
  }
  /* TODO: unlock */
  return ret;
}

void *TetrisTimerPthread::threadFunction(void *data)
{
  struct ThreadData *threadData = (struct ThreadData *) data;
  if (!threadData)
    return NULL;

  Tetris *tetris = threadData->tetris;
  int usec = threadData->msec * 1000;
  while (!threadData->stop) {
    usleep (usec);
    if (!tetris->getField()->timer()) {
      threadData->interrupt = true;
      return NULL;
    }
  }

  return threadData;
}

TetrisTimerPthread::TetrisTimerPthread(Tetris *tetris)
  : TetrisTimer(tetris), mData(tetris)
{
  ;
}

bool TetrisTimerPthread::start()
{
  mData.stop = false;
  if (pthread_create(&mThread, NULL, threadFunction, &mData))
    return false;
  return true;
}

bool TetrisTimerPthread::stop()
{
  mData.stop = true;
  return true;
}

void Tetris::run()
{
  InputType inputType;
  mTimer->start();
  while (1) {
    mDrawer->draw();
    inputType = mInputer->input();
    if (!mField->input(inputType))
      ; // TODO:
    if (inputType == INPUT_TYPE_QUIT)
      break;
    if (mTimer->isInterrupted())
      break;
  }
  mTimer->stop();
  mDrawer->gameover();
}
