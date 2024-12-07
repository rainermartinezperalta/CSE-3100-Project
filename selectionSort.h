#ifndef __SELECTIONSORT_H
#define __SELECTIONSORT_H

#include <stdlib.h>
#include <pthread.h>

typedef struct MTArray {
  int* numbers;
  int  size;
} MTArray;

#define MAXINT (0x7fffffff)

typedef struct Segment {
   int*  array;    // pointer to array of int.
   int from,to;    // start and end of segment being sorted [from,to)
   int    won;    // whether we won this round
   int   minAt;    // where the min of [from,to) is located (from <= minAt < to)
} Segment;

typedef struct Share {
   int best;          // best value across all threads
   Segment* winner;   // pointer to winning thread
   int   reporting;   // how many are reporting
   int   needed;      // how many are needed to complete a wave 
   int   gen;         // wave identifier
   int*  res;         // pointer to array where results go
   int   nbIn;        // how many results are in 
   pthread_mutex_t mtx;
   pthread_cond_t  done;
} Share;

typedef struct TArg {
   Segment* seg;  // Pointer to segment meant for the i^th thread. 
   Share*   shr;  // shared coordination
   pthread_t name;
} TArg;

MTArray* initArray(int* numbers, int n);
void* sortThread(void* args);
void checkArray(MTArray* a);
void printArray(MTArray* array);

#endif
