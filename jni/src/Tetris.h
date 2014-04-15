/**
 * @file Tetris.h
 * @author Hiroo MATSUMOTO <hiroom2.mail@gmail.com>
 */
#ifndef __TETRIS_H
#define __TETRIS_H

#include <iostream>
#include <cstdlib>
#include <climits>
#include <ctime>
#include <unistd.h>

class TetrisIndex {
 public:
  int c;
  int r;

  TetrisIndex() : c(0), r(0) {}
  TetrisIndex(int c, int r) : c(c), r(r) {}

  TetrisIndex(const TetrisIndex &index) {
    c = index.c;
    r = index.r;
  }

  TetrisIndex &operator=(const TetrisIndex &index) {
    c = index.c;
    r = index.r;
    return *this;
  }

  TetrisIndex &operator=(const TetrisIndex *index) {
    c = index->c;
    r = index->r;
    return *this;
  }

  static TetrisIndex rotate(const TetrisIndex &index) {
    TetrisIndex rotateIndex = TetrisIndex(-index.r, index.c);
    return rotateIndex;
  }

};

/**
 *  IIII JJJ LLL OO  SS TTT ZZ
 *         J L   OO SS   T   ZZ
 */

enum BarType {
  BAR_TYPE_E = ' ',
  BAR_TYPE_I = 'I',
  BAR_TYPE_J = 'J',
  BAR_TYPE_L = 'L',
  BAR_TYPE_O = 'O',
  BAR_TYPE_S = 'S',
  BAR_TYPE_T = 'T',
  BAR_TYPE_Z = 'Z',
};

enum {
  TETRIS_BAR_ROW = 4,
  TETRIS_BAR_COL = 4,
  TETRIS_BAR_NR = 7,
  TETRIS_BAR_START_COL = 1,
  TETRIS_BAR_START_ROW = 1,
  TETRIS_FIELD_ROW = 20,
  TETRIS_FIELD_COL = 10,
  TETRIS_FIELD_START_COL = 3,
  TETRIS_FIELD_START_ROW = TETRIS_BAR_ROW - 1,
};

enum InputType {
  INPUT_TYPE_EMPTY = 0,
  INPUT_TYPE_UP,
  INPUT_TYPE_DOWN,
  INPUT_TYPE_RIGHT,
  INPUT_TYPE_LEFT,
  INPUT_TYPE_ROT_RIGHT,
  INPUT_TYPE_ROT_LEFT,
  INPUT_TYPE_QUIT,
};

class TetrisBar {
 private:
  BarType mType;
  TetrisIndex **mIndex;
  int mIndexSize;
  int mRotSize;

  static inline int rc2point(int r, int c) {
    return TETRIS_BAR_COL * (r) + (c);
  }

  int countNotEmptyGrid(BarType type, const char *grid) {
    int ret = 0;
    for (int r = 0; r < TETRIS_BAR_ROW; ++r)
      for (int c = 0; c < TETRIS_BAR_COL; ++c)
        if (grid[rc2point(r, c)] == type)
          ret++;
    return ret;
  }

 public:
  explicit TetrisBar(BarType type, const char *str,
                     TetrisIndex rotStart, int rotSize);

  BarType getType() const { return mType; }
  int getIndexSize() const { return mIndexSize; }
  int getRotSize() const { return mRotSize; }

  const TetrisIndex getIndex(int bar, int rot = 0) const {
    return mIndex[rot][bar];
  }

#include <Tetris.def>
#define DEFINE_GET_BAR(type)                       \
  static const TetrisBar *getBar##type() {         \
    static const TetrisBar                         \
      TetrisBar##type(BAR_TYPE_##type,             \
                      BAR_TYPE_##type##_STRING,    \
                      BAR_TYPE_##type##_ROT_START, \
                      BAR_TYPE_##type##_ROT_SIZE); \
      return &TetrisBar##type;                     \
  }
  DEFINE_GET_BAR(E);
  DEFINE_GET_BAR(I);
  DEFINE_GET_BAR(J);
  DEFINE_GET_BAR(L);
  DEFINE_GET_BAR(O);
  DEFINE_GET_BAR(S);
  DEFINE_GET_BAR(T);
  DEFINE_GET_BAR(Z);
#undef DEFINE_GET_BAR
#define getBar(type) getBar##type()

};

class TetrisField {
 private:
  BarType mGrid[TETRIS_FIELD_ROW][TETRIS_FIELD_COL];
  int mRow;
  int mCol;

  const TetrisBar *mNextBar;
  int mNextBarRot;

  const TetrisBar *mBar;
  TetrisIndex mBarIndex;
  int mBarRot;

  unsigned mScore;
  unsigned mLines;

 public:
  TetrisField();
  ~TetrisField();

  int getRow() { return mRow; }
  int getCol() { return mCol; }

  void setRow(int row) { mRow = row; }
  void setCol(int col) { mCol = col; }

  bool input(InputType inputType);
  bool timer();

  bool checkLocatable(TetrisIndex &next, int rot);

  bool moveBar(int dx, int dy);
  bool moveUpBar() { return moveBar(0, -1); }
  bool moveDownBar() { return moveBar(0, +1); }
  bool moveLeftBar() { return moveBar(-1, 0); }
  bool moveRightBar() { return moveBar(+1, 0); }

  bool rotBar(int dr);
  bool rotLeftBar() { return rotBar(-1); }
  bool rotRightBar() { return rotBar(+1); }

  void putBar();
  bool checkLine(int row);
  void deleteLine(int row);
  void deleteLine();

  BarType getGrid(int r, int c) { return mGrid[r][c]; }
  void setGrid(int r, int c, BarType t) { mGrid[r][c] = t; }

  void clear() {
    for (int r = 0; r < mRow; ++r)
      for (int c = 0; c < mCol; ++c)
        setGrid(r, c, BAR_TYPE_E);
  }

  const TetrisBar *getBarFromType(int type) {
    switch (type) {
#define CASE(n, type) case n: { return TetrisBar::getBar(type); }
      CASE(0, I);
      CASE(1, J);
      CASE(2, L);
      CASE(3, O);
      CASE(4, S);
      CASE(5, T);
      CASE(6, Z);
#undef CASE
    default:
      break;
    }
    return NULL;
  }

  const TetrisBar *getNextBar() { return mNextBar; }
  int getNextBarRot() { return mNextBarRot; }

  void setNextBar(BarType type) { mNextBar = getBarFromType(type); }
  void setNextBarRot(int rot) { mNextBarRot = rot; }

  const TetrisBar *getBar() { return mBar; }
  TetrisIndex getBarIndex() { return mBarIndex; }
  int getBarRot() { return mBarRot; }

  void setBar(BarType type) { mBar = getBarFromType(type); }
  void setBarIndex(TetrisIndex index) { mBarIndex = index; }
  void setBarRot(int rot) { mBarRot = rot; }

  int rand(int max = 1) {
    return (int) (max * (::rand() / (RAND_MAX + 1.0f)));
  }

  const TetrisBar *getRandBar() {
    return getBarFromType(rand(TETRIS_BAR_NR));
  }

  int getRandBarRot(const TetrisBar *bar) { return rand(bar->getRotSize()); }

  bool setBar() {
    mBar = getNextBar();
    mBarIndex = TetrisIndex(TETRIS_FIELD_START_COL,
                            TETRIS_FIELD_START_ROW);
    mBarRot = getNextBarRot();

    mNextBar = getRandBar();
    mNextBarRot = getRandBarRot(mNextBar);

    /** BUG: If There is a bar at an upper place, moveUpBar will
        be failed */
    /** When rotating Tetris bar, there will be some upper spaces
        which should be deleted. */
    for (int r = 0; r < TETRIS_FIELD_START_ROW; ++r)
      moveUpBar();
    return checkLocatable(mBarIndex, mBarRot);
  }

  void setBarIndex(int x, int y) {
    mBarIndex.c = x;
    mBarIndex.r = y;
  }

  unsigned getScore() { return mScore; }
  unsigned getLines() { return mLines; }

  void setScore(int score) { mScore = score; }
  void setLines(int lines) { mLines = lines; }
};

class Tetris;

class TetrisDrawer {
 protected:
  Tetris *mTetris;

  virtual void drawFrame(TetrisField * field, int baseCol) = 0;
  virtual void drawField(TetrisField *field, int baseCol) = 0;
  virtual void drawBar(TetrisField *field, int baseCol) = 0;
  virtual void drawScore(TetrisField *field, int baseCol) = 0;
  virtual void drawNextBar(TetrisField *field, int baseCol) = 0;
  virtual void erase() = 0;
  virtual void update() = 0;

  void draw(TetrisField *field, int baseCol);

 public:
  TetrisDrawer(Tetris *tetris) : mTetris(tetris) {}
  virtual ~TetrisDrawer() {}
  virtual void gameover() = 0;

  void draw();
};

class TetrisInputer {
 protected:
  Tetris *mTetris;

 public:
  TetrisInputer(Tetris *tetris) : mTetris(tetris) {}
  virtual ~TetrisInputer() {}

  virtual InputType input() = 0;
};

class TetrisTimer {
 protected:
  Tetris *mTetris;

 public:
  TetrisTimer(Tetris *tetris) : mTetris(tetris) {}
  virtual ~TetrisTimer() {}

  virtual bool start() = 0;
  virtual bool stop() = 0;
  virtual bool isInterrupted() = 0;
};

#define TIMER_INTERVAL_MSEC (500)

struct ThreadData {
public:
  Tetris *tetris;
  bool stop;
  bool interrupt;
  int msec;

  ThreadData(Tetris *tetris)
  : tetris(tetris), stop(false), interrupt(false),
    msec(TIMER_INTERVAL_MSEC) {}
};

class TetrisTimerPthread : public TetrisTimer {
 private:
  ThreadData mData;
  pthread_t mThread;

  static void *threadFunction(void *data);

 public:
  TetrisTimerPthread(Tetris *tetris);
  ~TetrisTimerPthread() {}

  bool start();
  bool stop();
  bool isInterrupted() { return mData.interrupt; }
};

class Tetris {
 private:
  TetrisField *mField;
  TetrisDrawer *mDrawer;
  TetrisInputer *mInputer;
  TetrisTimer *mTimer;

 protected:
  Tetris() : mDrawer(NULL), mInputer(NULL), mTimer(NULL) {
    mField = new TetrisField();
  }

  virtual ~Tetris() {
    delete mField;
  }

  void registerDrawer(TetrisDrawer *drawer) { mDrawer = drawer; }
  void registerInputer(TetrisInputer *inputer) { mInputer = inputer; }
  void registerTimer(TetrisTimer *timer) { mTimer = timer; }

 public:
  void run();
  TetrisField *getField() { return mField; }
};

#endif /* __TETRIS_H */
