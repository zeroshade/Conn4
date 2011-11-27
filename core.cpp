/* 
 * File:   core.cpp
 * Author: zeroshade
 *
 * Created on November 2, 2011, 9:27 PM
 */

#include <iostream>
#include <ncurses.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <limits>
#include "core.h"
#include "Board.h"
#include "BitBoard.h"
#include "TranspositionTable.h"
/*
 * 
 */
using namespace std;

#define CONSOLE

static Board board;
static BitBoard bBoard;
static TranspositionTable tt;
static short moveprio[] = {3,2,5,1,4,0,6};

static int history[2][SIZE1];

void inithistory()
{
  for (int side=0; side<2; side++)
    for (int i=0; i<(WIDTH+1)/2; i++)
      for (int h=0; h<H1/2; h++)
        history[side][H1*i+h] = history[side][H1*(WIDTH-1-i)+HEIGHT-1-h] =
        history[side][H1*i+HEIGHT-1-h] = history[side][H1*(WIDTH-1-i)+h] =
         4+min(3,i) + max(-1,min(3,h)-max(0,3-i)) + min(3,min(i,h)) + min(3,h);
}

int minmaxexec(short depth, int alpha, int beta) {
    short status = NOTHING;
    if (depth == 0 || (status = board.gameover()) != NOTHING)
        return board.score(status);

    int b = beta;
    for (short i = 0; i < WIDTH; ++i) {
        short m = moveprio[i];
        bool valid = board.move(m);
        if (!valid) continue;
        int v = -minmaxexec(depth - 1, -b, -alpha);
        if (alpha < v && v < beta)
            v = -minmaxexec(depth - 1, -beta, -alpha);
        alpha = max(alpha, v);
        board.undo(m);
        if (beta <= alpha)
            return alpha;
        b = alpha + 1;
    }
    return alpha;
}

int minmax(short depth, int alpha, int beta) {
    int running = alpha;
    int decide = 0;
    for (short i = 0; i < WIDTH; ++i) {
        short m = moveprio[i];
#ifdef DEBUG
        if (board.depth == 0)
            cerr << "M : " << m << endl;
#endif
        bool valid = board.move(m);
        if (!valid) { if (decide == i) ++decide; continue; }
        int v = -minmaxexec(depth - 1, alpha, beta);
        board.undo(m);
        if (v > running) {
            running = v;
            decide = i;
        }
#ifdef DEBUG
        if (board.depth == 0)
            cerr << "v: " << v << ", a: " << alpha << ", b: " << beta << endl;
#endif
        if (beta <= running)
            break;
    }

    int k = moveprio[0];
    moveprio[0] = moveprio[decide];
    moveprio[decide] = k;

    return moveprio[0];
}

const int bh = HEIGHT * 2 + 1;
const int bw = WIDTH * 2 + 1;

void printBoard(int row, int col) {
#ifdef OLDBOARD
    istringstream str(board.show());
#else
    istringstream str(bBoard.show());
#endif
    string line;
    int i = 0;
    while (getline(str, line))
    #ifndef CONSOLE
        mvprintw((row - bh) / 2 + (i++), (col - bw) / 2, "%s", line.c_str());
    #else
        cout << line << endl;
    #endif
}

int negamax(const int maxdepth, int a, int b) {
    short mv[WIDTH];
    if (bBoard.gameover() || bBoard.curDepth() == maxdepth) // node is a terminal node or depth = 0
        return ((bBoard.turn())?-1:1) * bBoard.score();
    else {
        if (bBoard.curDepth() == SIZE - 1)
            return DRAW;
        bitboard opp = bBoard.rawBoard(bBoard.turn()^1);
        for (int i = 0; i < WIDTH; ++i) {
            mv[i] = i;
            bitboard next = opp | (bitboard)1 << bBoard.colHeight(i);
            if (!isValid(next))
                continue;
            if (isGameover(next)) {
                bitboard nextd = opp | (bitboard)2 << bBoard.colHeight(i);
                if (isValid(nextd) && isGameover(nextd))
                    return MIN; // double threat
                bBoard.move(i);
                int score = -negamax(maxdepth, -b, -a);
                bBoard.undo();
                return score;
            }
        }
        int ttscore = tt.transpose(bBoard);
        if (ttscore != UNKNOWN) {
            return ttscore;
        }
        int idx = tt.lastIndex();
        int lock = tt.lastLock();
        int best = -1;
        int score = MIN;
        int work = tt.getCalls();
        for (int i = 0; i < WIDTH; ++i) {
            int l = i;
            int val = history[bBoard.turn()][bBoard.colHeight(mv[l])];
            for (int j = i + 1; j < WIDTH; ++j) {
                int v = history[bBoard.turn()][bBoard.colHeight(mv[j])];
                if (v > val) {
                    val = v; l = j;
                }
            }
            int j;
            for (j = mv[l]; l > i; --l)
                mv[l] = mv[l-1];
            if (!bBoard.availMove(mv[i] = j))
                continue;
            bBoard.move(mv[i]);
            val = -negamax(maxdepth, -b, -a);
            bBoard.undo();

            if (val > score) {
                best = i;
                if ((score = val) > a && (a=val)>=b) {
                    if (best > 0) {
                        for (i = 0; i < best; ++i)
                            history[bBoard.turn()][bBoard.colHeight(mv[i])]--;
                        history[bBoard.turn()][bBoard.colHeight(mv[best])] += best;
                    }
                    break;
                }
            }
        }
        work = tt.getCalls() - work;
        int workval;
        for (workval = 0; (work >>= 1) != 0; ++workval);
        tt.transtore(idx, lock, score, workval);
        return score;
    }
}

int checkMoves(int max, int a, int b) {
    inithistory();
    int running = a;
    int decide = 0;
    short mv[WIDTH];
    for (short i = 0; i < WIDTH; ++i) mv[i] = i;
    for (short i = 0; i < WIDTH; ++i) {
        int l = i;
        int val = history[bBoard.turn()][bBoard.colHeight(mv[l])];
        for (int j = i + 1; j < WIDTH; ++j) {
            int v = history[bBoard.turn()][bBoard.colHeight(mv[j])];
            if (v > val) {
                val = v;
                l = j;
            }
        }
        int j;
        for (j = mv[l]; l > i; --l)
            mv[l] = mv[l - 1];
        bool valid = bBoard.availMove(mv[i] = j);
        if (!valid) { if (decide == i) ++decide; continue;  }
        bBoard.move(mv[i]);
        val = -negamax(max - 1, -b, -a);
        bBoard.undo();
        cout << "i: " << mv[i] << ", v: " << val << endl;
        if (val > running) {
            decide = i;
            if ((running = val) > a && (a = val) >= b) {
                if (decide > 0) {
                    for (i = 0; i < decide; ++i)
                        history[bBoard.turn()][bBoard.colHeight(mv[i])]--;
                    history[bBoard.turn()][bBoard.colHeight(mv[decide])] += decide;
                }
                break;
            }
        }
    }
    return mv[decide];
}

int makedecision() {
#ifdef OLDBOARD
    board.depth = 0;

    if (board.gameover() != NOTHING) return -1;

    int ret = board.quickmove();

    if (ret == -1)
        ret = minmax(MAXDEPTH, MIN, MAX);

    board.move(ret);
#else
    if (bBoard.gameover()) return -1;
    int ret = bBoard.quickmove();

    if (ret == -1)
        ret = checkMoves(MAXDEPTH + bBoard.curDepth(),MIN,MAX);

    cout << "Ret: " << ret << endl;
#endif
    return ret;
}

//#define OLDBOARD
int main(int argc, char** argv) {
    int row, col;
#ifndef CONSOLE
    initscr(); /* start the curses mode */
    getmaxyx(stdscr, row, col); /* get the number of rows and columns */
    const int bottom_row = (row/2) + HEIGHT * 2;
    const int bottom_col = (col - 20) / 2;
    char buf[80];
#else
    string buf;
#endif
    int play = numeric_limits<int>::max();
    printBoard(row, col);
#ifdef OLDBOARD
    board.setTurn(PLAYER);

    while ((play < 1 || play > WIDTH) || !board.move(play-1)) {
        if (play != numeric_limits<int>::max())
            mvprintw(LINES - 3, bottom_col, "Invalid move!");
        mvprintw(LINES - 2, bottom_col, "Choose column[1-%d]: ", WIDTH);
        getstr(buf);
        if (play != numeric_limits<int>::max())
            mvprintw(LINES - 3, bottom_col, "             ");
        if (string(buf) == "quit")
            exit(0);
        play = atoi(buf);
    }
    printBoard(row,col);
    play = makedecision();

    short end = NOTHING;
    while ((end = board.gameover()) == NOTHING) {
        printBoard(row, col);

        play = numeric_limits<int>::max();
        board.setTurn(PLAYER);
        while ((play < 1 || play > WIDTH) || !board.move(play - 1)) {
            if (play != numeric_limits<int>::max())
                mvprintw(LINES - 3, bottom_col, "Invalid move!");
            mvprintw(LINES - 2, bottom_col, "Choose column[1-%d]: ", WIDTH);
            getstr(buf);
            if (play != numeric_limits<int>::max())
                mvprintw(LINES - 3, bottom_col, "             ");
            if (string(buf) == "quit")
                exit(0);
            play = atoi(buf);
        }
        printBoard(row,col);
        refresh();
        play = makedecision();
    }
    printBoard(row, col);

    if (end == DRAW)
        mvprintw(LINES - 5, col / 2 - 2, "DRAW!");
    else if (board.curTurn() == PLAYER)
        mvprintw(LINES - 5, col / 2 - 6, "Computer Wins!");
    else
        mvprintw(LINES - 5, col / 2 - 6, "Player Wins!");
#else
    while ((play < 1 || play > WIDTH) || !bBoard.availMove(play-1)) {
        if (play != numeric_limits<int>::max())
#ifndef CONSOLE
            mvprintw(LINES - 3, bottom_col, "Invalid move!");
        mvprintw(LINES - 2, bottom_col, "Choose column[1-%d]: ", WIDTH);
        getstr(buf);
#else
        cout << "Invalid move!" << endl;
        cout << "Choose column[1-" << WIDTH << "]: ";
        cin >> buf;
#endif
        if (play != numeric_limits<int>::max())
#ifndef CONSOLE
            mvprintw(LINES - 3, bottom_col, "             ");
        if (string(buf) == "quit")
            exit(0);
        play = atoi(buf);
#else
        if (buf == "quit")
            exit(0);
        play = atoi(buf.c_str());
#endif
    }
    bBoard.move(play-1);
    printBoard(row, col);

    play = makedecision();
    while (!bBoard.gameover()) {
        bBoard.move(play);
        printBoard(row, col);
        if (bBoard.gameover()) break;

        play = numeric_limits<int>::max();
        while ((play < 1 || play > WIDTH) || !bBoard.availMove(play - 1)) {
            if (play != numeric_limits<int>::max())
#ifndef CONSOLE
                mvprintw(LINES - 3, bottom_col, "Invalid move!");
            mvprintw(LINES - 2, bottom_col, "Choose column[1-%d]: ", WIDTH);
            getstr(buf);
#else
            cout << "Invalid move!" << endl;
            cout << "Choose column[1-" << WIDTH << "]: ";
            cin >> buf;
#endif
#ifndef CONSOLE
            if (play != numeric_limits<int>::max())
                mvprintw(LINES - 3, bottom_col, "             ");
            if (string(buf) == "quit")
                exit(0);
            play = atoi(buf);
#else
            if (buf == "quit")
                exit(0);
            play = atoi(buf.c_str());
#endif
        }
        bBoard.move(play-1);
        printBoard(row,col);
        refresh();
        play = makedecision();
    }
#endif
#ifndef CONSOLE
    getch();
    endwin();
#endif
    return 0;
}

