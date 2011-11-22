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
/*
 * 
 */
using namespace std;

static Board board;
static BitBoard bBoard;
static short moveprio[] = {3,2,5,1,4,0,6};

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
        if (beta <= alpha)
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
        mvprintw((row - bh) / 2 + (i++), (col - bw) / 2, "%s", line.c_str());
}

int negamax(const int maxdepth, int a, int b) {
    if (bBoard.gameover() || bBoard.curDepth() == maxdepth) // node is a terminal node or depth = 0
        return ((bBoard.turn())?1:-1) * bBoard.score();
    else {
        bBoard.swapTurn();
        for (int i = 0; i < WIDTH; ++i) {
            if (!bBoard.availMove(i))
                continue;
            bBoard.move(i);
            if (bBoard.gameover()) {
                bBoard.undo();
                bBoard.swapTurn();
                return LOSS;
            }
            bBoard.undo();
        }
        bBoard.swapTurn();
        for (int i = 0; i < WIDTH; ++i) {
            bBoard.move(i);
            a = max(a, -negamax(maxdepth, -b, -a));
            bBoard.undo();
            if (a>=b)
                break;
        }
        return a;
    }
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
        ret = negamax(MAXDEPTH + bBoard.curDepth(),MIN,MAX);
#endif
    return ret;
}

//#define OLDBOARD
int main(int argc, char** argv) {
    int row, col;
    initscr(); /* start the curses mode */
    getmaxyx(stdscr, row, col); /* get the number of rows and columns */
    const int bottom_row = (row/2) + HEIGHT * 2;
    const int bottom_col = (col - 20) / 2;
    char buf[80];
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

    play = makedecison();

    printBoard(row, col);
#endif
    getch();
    endwin();

    return 0;
}

