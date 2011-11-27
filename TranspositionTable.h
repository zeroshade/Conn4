/* 
 * File:   TranspositionTable.h
 * Author: zeroshade
 *
 * Created on November 25, 2011, 11:40 PM
 */

#ifndef _TRANSPOSITIONTABLE_H
#define	_TRANSPOSITIONTABLE_H

#define LOCKSIZE 26
#define TRANSIZE 8306069
// should be a prime no less than about 2^{SIZE1-LOCKSIZE}, e.g.
// 4194301,8306069,8388593,15999961,33554393,67108859,134217689,268435399

#define SYMMREC 10 // symmetry normalize first SYMMREC moves

typedef struct {
#if (LOCKSIZE<=32)
  unsigned biglock:LOCKSIZE;
  unsigned bigwork:6;
  unsigned newlock:LOCKSIZE;
#else
  uint64 biglock:LOCKSIZE;
  unsigned bigwork:6;
  uint64 newlock:LOCKSIZE;
#endif
  unsigned newscore:3;
  unsigned bigscore:3;
} hashentry;

#include <sys/types.h>
typedef u_int64_t uint64;

class BitBoard;

class TranspositionTable {
public:
    TranspositionTable();
    TranspositionTable(const TranspositionTable& orig);
    virtual ~TranspositionTable();
    void emptyTable();
    int transpose(BitBoard& board);
    void transtore(int idx, unsigned lock, int score, int work);
    inline int lastIndex() { return index; }
    inline int lastLock() { return lock; }
    inline int getCalls() { return calls; }
private:
    void hash(BitBoard& board);
    hashentry* ht;
    unsigned lock, index;
    uint64 calls;
};

#endif	/* _TRANSPOSITIONTABLE_H */

