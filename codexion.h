#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_scheduler {
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_args
{
	long long	number_of_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	long long	number_of_compiles_required;
	long long	dongle_cooldown;
	t_scheduler	scheduler_type;
}	t_args;

typedef struct s_heap_node
{
	int			coder_number;
	long long	priority;
}	t_heap_node;

typedef struct s_heap
{
	t_heap_node	nodes[2];
	int			size;
}	t_heap;

typedef struct s_simulation	t_simulation;

typedef struct s_dongle
{
	int				number;
	short			dongle_is_available;
	long long		cooldown_end_time;
	int				left_coder;
	int				right_coder;
	t_heap			heap;
	pthread_mutex_t	dongle_mutex;
	pthread_cond_t	dongle_cond;
}	t_dongle;

typedef struct s_coder
{
	int					coder_number;
	long long			time_to_burnout;
	long long			time_since_last_compile;
	long long			deadline;
	int					compile_count;
	int					left_dongle;
	int					right_dongle;
	struct s_simulation	*sim;
}	t_coder;

typedef struct s_simulation
{
	t_args				args;
	t_dongle			*dongles;
	t_coder				*coders;
	pthread_mutex_t		sim_print_mutex;
	pthread_mutex_t		sim_mutex;
	pthread_cond_t		sim_cond;
	int					threads_at_barrier;
	short				stop_simulation;
	long long			start_time;
}	t_simulation;

#include "input_utils.h"
#include "args.h"
#include "error_message.h"
#include "utils_0.h"
#include "heap_utils.h"
#include "heap.h"
#include "dongle.h"
#include "init.h"
#include "simulation_utils.h"
#include "simulation.h"
#include "monitor.h"

#endif