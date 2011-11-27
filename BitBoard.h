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
    inline bool gameover() { return depth == SIZE || isGameover(color[turn()^1]); }
    inline bool valid() { return isValid(color[turn()^1]); }
    int score();
    inline int turn() { int i = depth & 1; return (swap) ? i ^ 1: i; }
    inline void swapTurn() { swap = !swap; }
    const bitboard& rawBoard(int side) { return color[side]; }
    inline bool availMove(int col) {
        bitboard tmp = color[turn()] | ((bitboard)1 << height[col]);
        return isValid(tmp);
    }
    void move(int col);
    inline int colHeight(int col) { return height[col]; }
    void undo();
    inline int curDepth() { return depth; }
    int quickmove();
    inline bitboard encode() {
        return color[curDepth()&1] + color[0] + color[1] + BOTTOM;
    }
private:
    bitboard color[2];  // black and white bitboard
    int moves[SIZE],depth;
    char height[WIDTH]; // holds bit index of lowest free square
    bool swap;
};

#endif	/* _BITBOARD_H */

