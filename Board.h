/* 
 * File:   Board.h
 * Author: zeroshade
 *
 * Created on November 2, 2011, 9:33 PM
 */

#ifndef _BOARD_H
#define	_BOARD_H

#include "core.h"
#include <string>

class Board {
public:
    Board();
    std::string show();
    bool move(short col);
    bool undo(short col);
    short gameover();
    int score(short status);
    void setTurn(short t) { turn = t; }
    short curTurn() { return turn; }
    int quickmove();
    short filled;
    short depth;
    short turn;
    short stack[WIDTH+1];
private:
    int try_to_win();
    int avoid_loss();
    int utility(short player);
    short connected(short col);
    short opp_connected(short col);
    short board[(WIDTH+1)*(HEIGHT+2)];
    short *groups[GROUPS][WINSIZE];
    short xplace[GROUPS][WINSIZE];
    short yplace[GROUPS][WINSIZE];
    short mlist[MAXSQ];
    short moves[MAXSQ];
};

static short weights[] = {16,4,2,8,4,2};

#endif	/* _BOARD_H */

