/* 
 * File:   BitBoard.cpp
 * Author: zeroshade
 * 
 * Created on November 4, 2011, 6:20 PM
 */

#include "BitBoard.h"
#include <sstream>

inline int bitcount(bitboard n) {
    int count = 0;
    while (n > 0) {
        count += (n & 1);
        n >>= 1;
    }
    return count;
}

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

int utility(bitboard& board, bool comp) {
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
    return n1*((comp)?2:1) + n2*((comp)?4:2) + n3*((comp)?16:8);
}

int BitBoard::score() {
    if (isGameover(color[1])) {
        return MAX;
    } else if (isGameover(color[0])) {
        return MIN;
    } else {
        int score = utility(color[1],true) - utility(color[0],false);
        return score;
    }
}

std::string BitBoard::show() {
    std::ostringstream str;
    for (int x = 0; x < (WIDTH * 2 + 1); ++x)
        str << "_"; // top row
    str << std::endl;
    bitboard n = 0;
    for (int i = 0; i < HEIGHT; ++i) {
        str << "|";
        for (int j = 0; j < WIDTH; ++j) {
            n = (bitboard)1<<((HEIGHT - 1) + (H1 * j)) - (i);
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
    int ret = -1;

    int middle = (WIDTH / 2);

    if (curDepth() == 0) {
        ret = middle;
    } else if (curDepth() == 1) {
        ret = middle;
        if (moves[0] == 1) ret = middle-1;
        else if (moves[0] == 5) ret = middle+1;
    } else if (curDepth() == (SIZE - 1)) {
        for (short i = 0; i < WIDTH; ++i) {
            if (height[i] < HEIGHT) {
                ret = i;
                break;
            }
        }
    } else {
        for (short i = 0; i < WIDTH && ret == -1; ++i) {
            bitboard check_me = color[turn()] | (bitboard)1 << height[i];
            if (isValid(check_me) && isGameover(check_me)) ret = i;
        }
        for (short i = 0; i < WIDTH && ret == -1; ++i) {
            bitboard check_opp = color[turn()^1] | (bitboard)1 << height[i];
            if (isValid(check_opp) && isGameover(check_opp)) ret = i;
        }
    }

    if (ret != -1) return ret;

    // opening book

    return -1;

}