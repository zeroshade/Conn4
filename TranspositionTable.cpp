/* 
 * File:   TranspositionTable.cpp
 * Author: zeroshade
 * 
 * Created on November 25, 2011, 11:40 PM
 */

#include "TranspositionTable.h"
#include <iostream>
#include "BitBoard.h"
#include <stdlib.h>

TranspositionTable::TranspositionTable() : lock(0), index(0), calls(0) {
  ht = (hashentry *)calloc(TRANSIZE, sizeof(hashentry));
  if (!ht) {
    std::cout << "Failed to allocate %lu bytes\n" << TRANSIZE*sizeof(hashentry) << std::endl;
    exit(0);
  }
}

TranspositionTable::TranspositionTable(const TranspositionTable& orig) {
}

TranspositionTable::~TranspositionTable() {
}

void TranspositionTable::emptyTable() {
  int i;

  for (i=0; i<TRANSIZE; i++) {
    ht[i] = (hashentry){0,0,0,0,0};
  }
}

int TranspositionTable::transpose(BitBoard& board) {
    hashentry he;

    hash(board);
    he = ht[index];
    if (he.biglock == lock)
        return he.bigscore;
    if (he.newlock == lock)
        return he.newscore;
    return UNKNOWN;
}

void TranspositionTable::hash(BitBoard& board) {
    bitboard tmp, temp = board.encode();

    if (board.curDepth() < SYMMREC) { // try symmetry by reversing columns
        bitboard temp2 = 0;
        for (tmp = temp; tmp != 0; tmp>>=H1)
            temp2 = temp2 << H1 | (tmp & COL1);
        if (temp2 < temp)
            temp = temp2;
    }
    lock = (unsigned int)(SIZE1>LOCKSIZE ? temp >> (SIZE1-LOCKSIZE) : temp);
    index = (unsigned int)(temp % TRANSIZE);
}

void TranspositionTable::transtore(int idx, unsigned lock, int score, int work) {
    hashentry he;

    calls++;

    he = ht[idx];
    if (he.biglock == lock || work >= he.bigwork) {
        he.biglock = lock;
        he.bigscore = score;
        he.bigwork = work;
    } else {
        he.newlock = lock;
        he.newscore = score;
    }
    ht[idx] = he;
}