#include "selectionSort.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void* sortThread(void* args) {
    TArg* targ = (TArg*)args;
    Segment* seg = targ->seg;
    Share* shr = targ->shr;
    int myGen = 0;

    while (seg->from < seg->to) {
        int local_min = MAXINT;
        seg->minAt = seg->from;

        for (int i = seg->from; i < seg->to; ++i) {
            if (seg->array[i] < local_min) {
                local_min = seg->array[i];
                seg->minAt = i;
            }
        }

        pthread_mutex_lock(&shr->mtx);
        if (shr->gen == myGen && local_min < shr->best) {
            shr->best = local_min;
            shr->winner = seg;
        }

        shr->reporting++;
        if (shr->reporting == shr->needed) {
            shr->res[shr->nbIn++] = shr->best;
            int tmp = shr->winner->array[shr->winner->from];
            shr->winner->array[shr->winner->from] = shr->best;
            shr->winner->array[shr->winner->minAt] = tmp;
            shr->winner->from++;
            shr->needed = (shr->winner->from == shr->winner->to) ? shr->needed - 1 : shr->needed;
            shr->gen++;
            shr->best = MAXINT;
            shr->reporting = 0;
            pthread_cond_broadcast(&shr->done);
        } else {
            while (shr->gen == myGen)
                pthread_cond_wait(&shr->done, &shr->mtx);
        }
        pthread_mutex_unlock(&shr->mtx);
        myGen++;
    }
    return NULL;
}

MTArray* initArray(int* numbers, int n) {
    MTArray* array = malloc(sizeof(MTArray));
    array->numbers = numbers;
    array->size = n;
    return array;
}

void printArray(MTArray* array) {
    for (int i = 0; i < array->size; i++)
        printf("%d ", array->numbers[i]);
    printf("\n");
}

void checkArray(MTArray* a) {
    int ok = 1;
    for (int i = 0; ok && i < a->size - 1; i++)
        ok = ok && a->numbers[i] <= a->numbers[i + 1];
    if (!ok) {
        printf("Not sorted...\n");
        exit(1);
    }
}
