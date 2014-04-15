/**
 * @file TetrisNcurses.h
 * @author Hiroo MATSUMOTO <hiroom2.mail@gmail.com>
 */
#ifndef __TETRISNCURSES_H
#define __TETRISNCURSES_H

#include <Tetris.h>
#include <ncurses.h>

class TetrisDrawerNcurses : public TetrisDrawer {
 protected:
  void drawFrame(TetrisField * field, int baseCol);
  void drawField(TetrisField *field, int baseCol);
  void drawBar(TetrisField *field, int baseCol);
  void drawScore(TetrisField *field, int baseCol);
  void drawNextBar(TetrisField *field, int baseCol);
  void erase() { ::erase(); }
  void update() { ::refresh(); }

  void drawGrid(int x, int y, const char *dot);
  void drawGrid(int x, int y, const char dot);
  void drawGrid(int x, int y, BarType type);
  void drawGrid(int x, int y, unsigned value);
  void drawFrameTopOrButtom(int row, int col, int baseCol);
  void drawFrameInner(int row, int col, int baseCol);
  void drawBar(const TetrisBar *bar, int rot, int r, int c);

 public:
  TetrisDrawerNcurses(Tetris *tetris);
  ~TetrisDrawerNcurses();
  void gameover();
};

class TetrisInputerNcurses : public TetrisInputer {
 public:
  TetrisInputerNcurses(Tetris *tetris);
  ~TetrisInputerNcurses() {}
  InputType input();
};

class TetrisNcurses : public Tetris {
 private:
  TetrisDrawerNcurses *mDrawer;
  TetrisInputerNcurses *mInputer;
  TetrisTimerPthread *mTimer;

 public:
  TetrisNcurses();
  ~TetrisNcurses();
};

#endif /* __TETRISNCURSES_H */
