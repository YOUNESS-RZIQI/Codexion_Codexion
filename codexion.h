#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "utils.h"
#include "args.h"
#include "error_messages.h"
#include "init.h"
#include "time.h"


typedef struct s_args
{
    long long number_of_coders;
    long long time_to_burnout;
    long long time_to_compile;
    long long time_to_debug;
    long long time_to_refactor;
    long long number_of_compiles_required;
    long long dongle_cooldown;
    char *scheduler;

} t_args;

typedef struct s_coder
{
	int					coder_number;
	long long			time_to_burnout;
	long long			time_since_last_compile;
	long long			deadline;
	int					compile_count;
}	t_coder;


/*

    parent = (i - 1) / 2
    left   = 2 * i + 1
    right  = 2 * i + 2


    * heapfy up : add + keep up if smaller then the parent
    * heapfy down : remove head + move last to head + swap with the smaller child if shiled is smaller.

    * min heap : smallest value is at the root
    * max heap : largest value is at the root

    Methods : 

        1) Insert : when you insert you garantee that the smalest is at the root.
        
        2) Extract min : it is the root so take it and use the heapfy down.
        

    heap struct :



*/

typedef struct s_heap
{
    int     *heap;
    int     size;
    int     capacity;
} t_heap;


typedef struct s_dongle
{
	int				number;
	short			dongle_is_available;
	long long		how_much_to_rest;
	long long		toked_at;
	int     		toked_by;
	pthread_mutex_t	dongle_mutex;
	pthread_cond_t	dongle_cond;
    
}	t_dongle;

typedef struct s_simulation
{
	t_args				args;
	t_dongle			*dongles;
	t_coder				*coders;
	pthread_mutex_t		sim_print_mutex;
	pthread_mutex_t		sim_mutex;
	pthread_cond_t		sim_cond;
	int					threads_at_barrier;
	short		        stop_simulation;
}



#endif