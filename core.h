/* 
 * File:   core.h
 * Author: zeroshade
 *
 * Created on November 2, 2011, 9:28 PM
 */

#ifndef _CORE_H
#define	_CORE_H

template <typename C, typename T>
C min(const C& x, const T& y) {
    return (x<y)?x:y;
}

template <typename C, typename T>
C max(const C& x, const T& y) {
    return (x>y)?x:y;
}

#define MIN -1000L
#define MAX 1000L
#define MAXDEPTH 16

#define YES 1
#define NO 0
#define NOTHING 0
#define LOSS 1
#define DRAWLOSS 2
#define DRAW 3
#define DRAWWIN 4
#define WIN 5
#define LOSSWIN 6
#define UNKNOWN -2000

#define FULL -1
#define EMPTY 0
#define COMPUTER 1
#define PLAYER 2
#define SWITCH 3

#define HEIGHT 6
#define WIDTH 7

#define MAXSQ (HEIGHT*WIDTH)
#define WINSIZE 4
#define GROUPS 69

#define SQM(x,y) ((x)+((y)<<3))
#define SQX(n) ((n)&7)
#define SQY(n) ((n)>>3)

// bitmask corresponds to board as follows in 7x6 case:
//  .  .  .  .  .  .  .  TOP
//  5 12 19 26 33 40 47
//  4 11 18 25 32 39 46
//  3 10 17 24 31 38 45
//  2  9 16 23 30 37 44
//  1  8 15 22 29 36 43
//  0  7 14 21 28 35 42  BOTTOM
#define H1 (HEIGHT+1)
#define H2 (HEIGHT+2)
#define SIZE (HEIGHT*WIDTH)
#define SIZE1 (H1*WIDTH)

#include <sys/types.h>
typedef u_int64_t uint64;
typedef int64_t int64;
#if (SIZE1<=64)
  typedef uint64 bitboard;
#else
  typedef __int128_t bitboard;
#endif

#define COL1 (((bitboard)1<<H1)-(bitboard)1)
#if (SIZE1 != 64)
#define ALL1 (((bitboard)1<<SIZE1)-(bitboard)1)
#define BOTTOM (ALL1 / COL1) // has bits i*H1 set
#elif (WIDTH==8 && HEIGHT==7) // one of exceptional cases
#define BOTTOM 0x0101010101010101LL
#else
# add definition for missing (weird) exceptional cases
#endif
#define TOP (BOTTOM << HEIGHT)

#endif	/* _CORE_H */

