/* 
 * File:   Board.cpp
 * Author: zeroshade
 * 
 * Created on November 2, 2011, 9:33 PM
 */

#include "Board.h"
#include <string>
#include <sstream>

Board::Board() {
    /* Initialize Groups */

    // Horizontal
    short i = 0;
    for (short y = 0; y < HEIGHT;++y) {
        for (short x = 0; x < WIDTH-3; ++x) {
            for (short j = 0; j < 4; ++j) {
                groups[i][j] = &board[SQM(x+j,y)];
                xplace[i][j] = x+j;
                yplace[i][j] = y;
            }
            ++i;
        }
    }

    // Vertical
    for (short y = 0; y < HEIGHT-3; ++y) {
        for (short x = 0; x < WIDTH; ++x) {
            for (short j = 0; j < 4; ++j) {
                groups[i][j] = &board[SQM(x,y+j)];
                xplace[i][j] = x;
                yplace[i][j] = y+j;
            }
            ++i;
        }
    }

    // Diagonal NE
    for (short y = 0; y < HEIGHT-3; ++y) {
        for (short x = 0; x < WIDTH-3; ++x) {
            for (short j = 0; j < 4; ++j) {
                groups[i][j] = &board[SQM(x+j,y+j)];
                xplace[i][j] = x+j;
                yplace[i][j] = y+j;
            }
            ++i;
        }
    }

    // Diagonal SE
    for (short y = 3; y < HEIGHT; ++y) {
        for (short x = 0; x < WIDTH-3; ++x) {
            for (short j = 0; j < 4; ++j) {
                groups[i][j] = &board[SQM(x+j,y-j)];
                xplace[i][j] = x+j;
                yplace[i][j] = y-j;
            }
            ++i;
        }
    }

    for (short x = 0; x < WIDTH; ++x)
        for (short y = 0; y < HEIGHT; ++y)
            board[SQM(x,y)] = EMPTY;

    for (short x = 0; x < 8; ++x) {
        board[SQM(7,x)] = FULL;
        board[SQM(x,6)] = board[SQM(x,7)] = FULL;
    }

    stack[WIDTH] = FULL;

    for (short x = 0; x < WIDTH; ++x)
        stack[x] = 0;

    turn = COMPUTER;
    filled = 0;

}

bool Board::move(short col) {
    if (stack[col] >= HEIGHT) return false;

    board[SQM(col,stack[col]++)] = turn;
    moves[filled] = col;
    mlist[filled] = SQM(col,stack[col]);
    turn ^= SWITCH;
    ++filled;
    ++depth;

    return true;
}

bool Board::undo(short col) {
    if (stack[col] < 1) return false;

    board[SQM(col,--stack[col])] = EMPTY;
    turn ^= SWITCH;
    --filled;
    moves[filled] = FULL;
    mlist[filled] = FULL;
    --depth;

    return true;
}

int Board::quickmove() {
    int ret = -1;
    if (filled == 0) {
        ret = 3;
    } else if (filled == 1) {
        ret = 3;
        if (moves[0] == 1) ret = 2;
        else if (moves[0] == 5) ret = 4;
    } else if (filled == MAXSQ - 1) {
        for (short i = 0; i < WIDTH; ++i) {
            if (stack[i] < HEIGHT) {
                ret = i;
                break;
            }
        }
    }

    if (ret != -1) return ret;

    if ((ret = try_to_win()) != -1) return ret;
    else if ((ret = avoid_loss()) != -1) return ret;

    // opening book

    return -1;
}

int Board::try_to_win() {
    int ret = -1;
    for(int x = 0; x < WIDTH; x++) {
        if (!move(x))
            continue;
        if(stack[x] <= HEIGHT && connected(x) >= 4) ret=x;
        undo(x);
    }
    return ret;
}

int Board::avoid_loss() {
    int ret = -1;
    turn ^= SWITCH;
    ret = try_to_win();
    turn ^= SWITCH;
    return ret;
}

short Board::gameover() {
    for (short i = 0; i < GROUPS; ++i) {
        if (*groups[i][0] != EMPTY &&
            *groups[i][0] == *groups[i][1] &&
            *groups[i][0] == *groups[i][2] &&
            *groups[i][0] == *groups[i][3]) return WIN;
    }

    return (filled == MAXSQ) ? DRAW : NOTHING;
}

int Board::score(short status) {
    if (status == NOTHING) {
        int sc = utility(turn);
        sc += utility(turn ^ SWITCH);
        return sc;
    } else if (status == DRAW)
        return 0;
    else {
        for (short i = 0; i < GROUPS; ++i) {
            if (*groups[i][0] != EMPTY &&
                *groups[i][0] == *groups[i][1] &&
                *groups[i][0] == *groups[i][2] &&
                *groups[i][0] == *groups[i][3])
                return (*groups[i][0] == COMPUTER) ? MAX : MIN;
        }

    }
}

int Board::utility(short player) {
    int ret = 0;
    short px,py,c;
    short col = moves[filled - 1];
    short idx = (player == COMPUTER) ? 0 : 3;

    // horizontal left
    c = 0;
    px = col;
    py = stack[col] - 1;
    while(c <= 3 && py >=0 && board[SQM(px,py)] == player) {
	++c; --py;
        ret += weights[idx + 3 - c];
    }

    // horizontal right
    c = 0;
    px = col+1;
    while(c <= 3 && px < WIDTH && board[SQM(px,py)] == player) {
        ++c; ++px;
        ret += weights[idx + 3 - c];
    }

    /* Check NW - SE diagonal */
    c = 0;
    px = col - 1;
    py = stack[col]+1;
    while(c <= 3 && px >= 0 && py < HEIGHT && board[SQM(px,py)] == player) {
        ++c; --px; ++py;
        ret += weights[idx + 3 - c];
    }

    c = 0;
    px = col + 1;
    py = stack[col] - 1;
    while(c <= 3 && px < WIDTH && py >= 0 && board[SQM(px,py)] == player) {
	++c; ++px; --py;
        ret += weights[idx + 3 - c];
    }

    /* Check NE - SW */
    c = 0;
    px = col - 1;
    py = stack[col] - 1;
    while(c <= 3 && px >= 0 && py>=0 && board[SQM(px,py)] == player) {
	++c; --px; --py;
        ret += weights[idx + 3 - c];
    }

    c = 0;
    px = col + 1;
    py = stack[col]+1;
    while(c <= 3 && px < WIDTH && py < HEIGHT && board[SQM(px,py)] == player) {
	++c; ++px; ++py;
        ret += weights[idx + 3 - c];
    }

    return ret;
}

short Board::connected(short col) {
    short maxcon=1;
    short px,py,connect;

    /* Check vertical */

    connect = 1;
    px = col;
    py = stack[col]-2;
    while(py >=0 && board[SQM(px,py)] == turn) {
	++connect;
	--py;
    }
    maxcon = max(connect,maxcon);

    /* Check horizontal */

    connect = 1;
    px = col - 1;
    py = stack[col] - 1;

    while(px >= 0 && board[SQM(px,py)] == turn) {
	++connect;
	--px;
    }

    px = col+1;
    while(px < WIDTH && board[SQM(px,py)] == turn) {
        ++connect;
	++px;
    }
    maxcon = max(connect,maxcon);

    /* Check NW - SE diagonal */
    connect = 1;
    px = col - 1;
    py = stack[col];
    while(px >= 0 && py < HEIGHT && board[SQM(px,py)] == turn) {
        ++connect;
	--px;
	++py;
    }
    px = col + 1;
    py = stack[col] - 2;
    while(px < WIDTH && py >= 0 && board[SQM(px,py)] == turn) {
	++connect;
	++px;
	--py;
    }
    maxcon = max(connect,maxcon);

    /* Check NE - SW */
    connect = 1;

    px = col - 1;
    py = stack[col] - 2;

    while(px >= 0 && py>=0 && board[SQM(px,py)] == turn) {
	++connect;
	--px;
	--py;
    }
    px = col + 1;
    py = stack[col];
    while(px < WIDTH && py < HEIGHT && board[SQM(px,py)] == turn) {
	++connect;
	++px;
	++py;
    }
    maxcon = max(connect,maxcon);

    return maxcon;
}

short Board::opp_connected(short col) {
    short con;
    turn ^= SWITCH;
    con = connected(col);
    turn ^= SWITCH;

    return con;
}

char text(short input) {
    switch (input) {
        case COMPUTER:
            return 'x';
        case PLAYER:
            return 'o';
        case EMPTY:
        default:
            return ' ';
    }
}

std::string Board::show() {
    std::ostringstream str;
    for (int x = 0; x < (WIDTH * 2 + 1); ++x)
        str << "_"; // top row
    str << std::endl;
    for (int x = HEIGHT - 1; x >= 0; --x) // prints the  board
    {
        str << "|";
        for (int j = 0; j < WIDTH; ++j)
            str << text(board[SQM(j,x)]) << "|";
        str << std::endl;
    }
    for (int x = 0; x < (WIDTH * 2 + 1); ++x)
        str << "-"; // bottom row
    str << std::endl;
    return str.str();
}