#include "selectionSort.h"
#include <assert.h>
#include <stdio.h>

int* parSelSort(int* numbers,int n,int nt) { // creates a sorted version of numbers in a newly allocated array
  Segment segs[nt];                          // create nt segments
  
  int from = 0,sz = n / nt, rem = n % nt;
  for(int i=0;i < nt;i++) {                  // allocate numbers to each segment
     Segment si = {numbers,from,from + sz + (i < rem)};
     segs[i] = si;
     from += sz + (i < rem);
  }
  
  Share share;                               // Create an instance of the shared structure
  share.winner = NULL;                       // Upon initialization, there is no winner
  share.reporting = 0;                       // no threads have reported yet
  share.needed    = nt;                      // total threads to enter the barrier must be nt
  share.gen       = 0;                       // Upon initialization, the generation is 0
  share.res       = malloc(sizeof(int)*n);   // array to store the result
  share.nbIn      = 0;                       // none of the threads have shared a result
  pthread_mutex_init(&share.mtx,NULL);
  pthread_cond_init(&share.done,NULL);

  TArg ta[nt];                               // create nt number of threads. The thread id's are stored in the array ta
  for(int i=0;i<nt;i++) {
     ta[i].seg = segs + i;                  // each thread gets its segment and a ref to the shared struct
     ta[i].shr = &share;     
     pthread_create(&ta[i].name, NULL, sortThread,ta+i);        // each thread calls the sortThread function
  }  

  for(int i = 0; i < nt; i++)
     pthread_join(ta[i].name, NULL);                // join the threads
     
  return share.res;
}

int main(int argc,char* argv[])
{
  if (argc < 3) {
    printf("usage: selectionSort <arraySize> <NBThreads>\n");
    exit(-1);
  }
  int n = atoi(argv[1]);                // number of values to be sorted
  int nt = atoi(argv[2]);               // number of threads
  int ub = 1000000;                     
  int* numbers = malloc(sizeof(int)*n);

  int* h0 = calloc(ub,sizeof(int));
  for(int i = 0; i < n; i++) {
     numbers[i] = random() % ub;
     h0[numbers[i]] += 1;               // only used to compare input with the the entries in the output to ensure correctness
  }

  int* resPtr = parSelSort(numbers,n,nt);
  MTArray* res = initArray(resPtr,n);
  // Print and check output correctness.
  printArray(res);
  checkArray(res);

  int* h1 = calloc(ub,sizeof(int));
  for(int i = 0; i < n; i++) {
     h1[res->numbers[i]] += 1;
  }

  for(int i=0;i < ub;i++) {
     if (h0[i] != h1[i]) {
        printf("Bad!\n");
        exit(2);
     }
  }
  
  free(h0);
  free(h1);
  free(numbers);
  free(resPtr);
  free(res);
  return 0;
}
