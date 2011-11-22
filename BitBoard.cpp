/* 
 * File:   BitBoard.cpp
 * Author: zeroshade
 * 
 * Created on November 4, 2011, 6:20 PM
 */

#include "BitBoard.h"
#include <sstream>

inline int pop4(unsigned long long x, unsigned long long y,
                unsigned long long u, unsigned long long v)
{
   static long long m1 = 0x5555555555555555LL,
         m2 = 0x3333333333333333LL,
         m3 = 0x0F0F0F0F0F0F0F0FLL,
         m4 = 0x000000FF000000FFLL;

    x = x - ((x >> 1) & m1);
    y = y - ((y >> 1) & m1);
    u = u - ((u >> 1) & m1);
    v = v - ((v >> 1) & m1);
    x = (x & m2) + ((x >> 2) & m2);
    y = (y & m2) + ((y >> 2) & m2);
    u = (u & m2) + ((u >> 2) & m2);
    v = (v & m2) + ((v >> 2) & m2);
    x = x + y;
    u = u + v;
    x = (x & m3) + ((x >> 4) & m3);
    u = (u & m3) + ((u >> 4) & m3);
    x = x + u;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x & m4;
    x = x + (x >> 32);
    return x & 0x000001FF;
}

inline int bitcount(bitboard& num) { return pop4(num, num^1, ~num, num|1); }

BitBoard::BitBoard() : swap(false) { reset(); }

BitBoard::BitBoard(const BitBoard& orig) {
}

BitBoard::~BitBoard() {
}

void BitBoard::reset() {
  depth = 0;
  color[0] = color[1] = 0;
  for (int i=0; i<WIDTH; i++)
    height[i] = (char)(H1*i);
}

bool isGameover(bitboard& newboard) {
    bitboard diag1 = newboard & (newboard>>HEIGHT);
    bitboard hori = newboard & (newboard>>H1);
    bitboard diag2 = newboard & (newboard>>H2);
    bitboard vert = newboard & (newboard>>1);
    return ((diag1 & (diag1 >> 2*HEIGHT)) |
            (hori & (hori >> 2*H1)) |
            (diag2 & (diag2 >> 2*H2)) |
            (vert & (vert >> 2)));
}

void BitBoard::move(int col) {
  color[turn()] ^= (bitboard)1<<height[col]++;
  moves[depth++] = col;
}

void BitBoard::undo() {
    int n = moves[--depth];
    color[turn()] ^= (bitboard)1<<--height[n];
}

int utility(bitboard& board) {
    int n1 = bitcount(board);
    bitboard vert = board & (board>>1);
    bitboard vert2 = (vert & (vert >> 1));
    int n3 = bitcount(vert2);
    int n2 = bitcount(vert);
    bitboard horiz = board & (board>>H1);
    bitboard horiz2 = (horiz & (horiz >> H1));
    n3 += bitcount(horiz2);
    n2 += bitcount(horiz);
    bitboard diag1 = board & (board>>HEIGHT);
    bitboard diag2 = board & (board>>H2);
    bitboard diag1_2 = diag1 & (diag1 >> HEIGHT);
    bitboard diag2_2 = diag2 & (diag2>>H2);
    n3 += bitcount(diag1_2);
    n3 += bitcount(diag2_2);
    n2 += bitcount(diag1);
    n2 += bitcount(diag2);
    return n1 + n2*4 + n3*16;
}

int BitBoard::score() {
    if (isGameover(color[turn()])) {
        return LOSS;
    } else if (isGameover(color[turn()^1])) {
        return WIN;
    } else {
        int score = utility(color[turn()^1]) - utility(color[turn()]);
    }
}

std::string BitBoard::show() {
    std::ostringstream str;
    for (int x = 0; x < (WIDTH * 2 + 1); ++x)
        str << "_"; // top row
    str << std::endl;
    bitboard n;
    for (int i = 0; i < HEIGHT; ++i) {
        str << "|";
        for (int j = 0; j < WIDTH; ++j) {
            n = 1<<((HEIGHT - 1) + (H1 * j));
            str << ((color[0] & n) ? 'x' : (color[1] & n) ? 'o' : ' ');
            str << "|";
        }
        str << std::endl;
    }
    for (int x = 0; x < (WIDTH * 2 + 1); ++x)
        str << "-"; // bottom row
    str << std::endl;
    return str.str();
}

int BitBoard::quickmove() {
    
}