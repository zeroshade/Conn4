/* 
 * File:   BitBoard.h
 * Author: zeroshade
 *
 * Created on November 4, 2011, 6:20 PM
 */

#ifndef _BITBOARD_H
#define	_BITBOARD_H

#include "core.h"
#include <string>

bool isGameover(bitboard& board);
inline bool isValid(bitboard& board) { return (board & TOP) == 0; }

class BitBoard {
public:
    BitBoard();
    BitBoard(const BitBoard& orig);
    virtual ~BitBoard();
    std::string show();
    void reset();
    inline bool gameover() { return isGameover(color[turn()^1]); }
    inline bool valid() { return isValid(color[turn()^1]); }
    int score();
    inline int turn() { int i = depth & 1; return (swap) ? depth ^ 1: depth; }
    inline void swapTurn() { swap = !swap; }
    const bitboard& rawBoard(int side) { return color[side]; }
    inline bool availMove(int col) {
        bitboard tmp = color[turn()] | ((bitboard)1 << height[col]);
        return isValid(tmp);
    }
    void move(int col);
    void undo();
    inline int curDepth() { return depth; }
    int quickmove();
private:
    bitboard color[2];  // black and white bitboard
    int moves[SIZE],depth;
    char height[WIDTH]; // holds bit index of lowest free square
    bool swap;
};

#endif	/* _BITBOARD_H */

