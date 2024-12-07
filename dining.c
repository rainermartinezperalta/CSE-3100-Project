#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "dining.h"

void small_group_dining(group_t *grp) {
    restaurant_t *restt = grp->restaurant;
    int table_size;

    pthread_mutex_lock(&restt->mutex);

    while ((restt->n_small_taken >= restt->n_small_tables) && 
           ((restt->n_big_taken >= restt->n_big_tables) || (restt->n_big_waiting > 0))) {
        restt->n_small_waiting++;
        PRINT_WAITING_MSG(grp);
        pthread_cond_wait(&restt->cond_small, &restt->mutex);
        restt->n_small_waiting--;
    }

    if (restt->n_small_taken < restt->n_small_tables) {
        table_size = SIZE_SMALL;
        restt->n_small_taken++;
    } else {
        table_size = SIZE_BIG;
        restt->n_big_taken++;
    }

    PRINT_SEATED_MSG(grp, table_size);
    pthread_mutex_unlock(&restt->mutex);

    DINING(grp);

    pthread_mutex_lock(&restt->mutex);
    if (table_size == SIZE_SMALL) {
        restt->n_small_taken--;
        pthread_cond_signal(&restt->cond_small);
    } else {
        restt->n_big_taken--;
        if (restt->n_big_waiting > 0)
            pthread_cond_signal(&restt->cond_big);
        else
            pthread_cond_signal(&restt->cond_small);
    }

    PRINT_LEAVING_MSG(grp, table_size);
    pthread_mutex_unlock(&restt->mutex);
}

void big_group_dining(group_t *grp) {
    restaurant_t *restt = grp->restaurant;

    pthread_mutex_lock(&restt->mutex);

    while (restt->n_big_taken >= restt->n_big_tables) {
        restt->n_big_waiting++;
        PRINT_WAITING_MSG(grp);
        pthread_cond_wait(&restt->cond_big, &restt->mutex);
        restt->n_big_waiting--;
    }

    restt->n_big_taken++;
    PRINT_SEATED_MSG(grp, SIZE_BIG);
    pthread_mutex_unlock(&restt->mutex);

    DINING(grp);

    pthread_mutex_lock(&restt->mutex);
    restt->n_big_taken--;
    if (restt->n_big_waiting > 0)
        pthread_cond_signal(&restt->cond_big);
    else
        pthread_cond_signal(&restt->cond_small);

    PRINT_LEAVING_MSG(grp, SIZE_BIG);
    pthread_mutex_unlock(&restt->mutex);
}

void *thread_grp(void *arg_orig) {
    group_t *grp = arg_orig;

    unsigned short g_random_buffer[3];
    if (grp->seed) {
        int actual_seed = (grp->seed << 10) + grp->id; 
        RANDOM_INIT(actual_seed);
    } else {
        int actual_seed = 3100 * grp->id + 12345;
        RANDOM_INIT(actual_seed);
    }

    for (int i = 0; i < grp->n_meals; i++) {
        if (grp->seed) {
            WAITING();
        }
        grp->time_to_dine = RANDOM_INT();
        PRINT_ARRIVING_MSG(grp);

        if (grp->sz == SIZE_SMALL)
            small_group_dining(grp);
        else 
            big_group_dining(grp);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc < 6) {
        printf("The correct usage is: ./dining num_small_groups num_big_groups n_small_tables n_big_tables n_meals.\n");
        exit(-1);
    }

    int n_small_groups = atoi(argv[1]);
    int n_big_groups = atoi(argv[2]);
    int n_small_tables = atoi(argv[3]);
    int n_big_tables = atoi(argv[4]);
    int n_meals = atoi(argv[5]);
    int seed = 3100;

    printf("Options: -g%d -G%d -t%d -T%d -m%d\n", 
            n_small_groups, n_big_groups, n_small_tables, n_big_tables, n_meals);

    restaurant_t restt = {0};
    restt.n_big_tables = n_big_tables;
    restt.n_small_tables = n_small_tables;
    pthread_mutex_init(&restt.mutex, NULL);
    pthread_cond_init(&restt.cond_small, NULL);
    pthread_cond_init(&restt.cond_big, NULL);

    int nbT = n_big_groups + n_small_groups;
    pthread_t tid[nbT];
    group_t grps[nbT];

    for (int i = 0; i < nbT; i++) {
        grps[i] = (group_t){i, SIZE_SMALL, n_meals, seed, 0, &restt};
        if (i >= n_small_groups) {      
            grps[i].sz = SIZE_BIG;
        }
        pthread_create(&tid[i], NULL, thread_grp, &grps[i]);
    }

    for(int i = 0; i < nbT; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&restt.mutex);
    pthread_cond_destroy(&restt.cond_small);
    pthread_cond_destroy(&restt.cond_big);

    return 0;
}
