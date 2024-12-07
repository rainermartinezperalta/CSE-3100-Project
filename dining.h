/* Please DO NOT modify this file */

#ifndef __DINING_H
#define __DINING_H

/* Structure to store information for the restaurant */
typedef struct restaurant_t_tag {
    int     n_small_tables;         // number of small tables in the restaurant 
    int     n_small_taken;          // number of small tables taken
    int     n_small_waiting;        // number of small groups waiting
    int     n_big_tables;           // number of big tables in the restaurant
    int     n_big_taken;            // number of big tables taken
    int     n_big_waiting;          // number of big groups waiting
    pthread_mutex_t mutex;          // mutex to protect shared data
    pthread_cond_t  cond_big;       // condition variable for big groups
    pthread_cond_t  cond_small;     // condition variable for small groups
} restaurant_t;

/* Structure to store information for a group */
typedef struct group_t {
    int id;                         // group id
    int sz;                         // group size, SIZE_SMALL or SIZE_BIG
    int n_meals;                    // number of meals the group will have
    int seed;                       // seed for random numbers
    int time_to_dine;               // time_to_dine
    restaurant_t  * restaurant;     // pointer to restaurant
} group_t;

/* Macros for setting random time to sleep */
enum {SIZE_SMALL, SIZE_BIG}; 

// Use the following macro in your thread function before any calls to get random values.
#define RANDOM_INIT(x)  (g_random_buffer[0]=3100,g_random_buffer[1]=(x),g_random_buffer[2] = (x) >> 16)
#define RANDOM_INT() (nrand48(g_random_buffer))
void my_msleep(int r) {
    struct timespec req = {0, (r % 50 + 1) * 1000000};
    nanosleep(&req, NULL);
}
#define WAITING() my_msleep(RANDOM_INT())
#define DINING(grp) my_msleep((grp)->time_to_dine)

/* Macros for printing messages */
#define SIZE_STR(sz)  ((sz) == SIZE_SMALL ? "small" : "big")
#define PRINT_ARRIVING_MSG(grp) printf("group %2d, %5s, is arriving...\n", (grp)->id, SIZE_STR((grp)->sz))
#define PRINT_WAITING_MSG(grp)  printf("group %2d, %5s, is waiting...\n", (grp)->id, SIZE_STR((grp)->sz))
#define PRINT_SEATED_MSG(grp, tsz)   printf("group %2d, %5s, is seated at a %s table...\n", (grp)->id, SIZE_STR((grp)->sz), SIZE_STR(tsz))
#define PRINT_LEAVING_MSG(grp, tsz)  printf("group %2d, %5s, is leaving from a %s table...\n", (grp)->id, SIZE_STR((grp)->sz), SIZE_STR(tsz))

#endif