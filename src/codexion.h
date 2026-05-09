/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:24 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:25 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdbool.h>

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
	int			compile_count;
}	t_heap_node;

typedef struct s_heap
{
	t_heap_node	nodes[2];
	int			size;
}	t_heap;

typedef struct s_simulation	t_simulation;

typedef struct s_dongle
{
	bool			dongle_is_available;
	long long		cooldown_end_time;
	t_heap			heap;
	pthread_mutex_t	dongle_mutex;
	pthread_cond_t	dongle_cond;
}	t_dongle;

typedef struct s_coder
{
	int					coder_number;
	long long			time_to_burnout;
	long long			time_since_last_compile_start;
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
	bool				stop_simulation;
	long long			start_time;
}	t_simulation;

bool			is_full_digit(char *s);

bool			ft_isdigit(char c);

long long		ft_atoi(const char *s);

bool			is_empty_args(t_args args);

t_args			empty_args(void);

t_args			convert_args(int argc, char **argv);

bool			null_error_message(void);

bool			input_error_message(void);

bool			initialize_simulation_error_message(void);

bool			mutex_error_message(void);

bool			thread_creation_fail_error_message(void);

long long		get_current_time_ms(void);

void			wake_all_dongles(t_simulation *sim);

bool			wait_at_barrier(t_simulation *sim);

void			init_coders_and_dongles(t_simulation *sim);


void			heapify_up(t_simulation *sim, t_heap *heap, int i);

void			heapify_down(t_heap *heap);


int				heap_insert(t_simulation *sim, t_heap *heap, t_heap_node node,
					t_scheduler type);

t_heap_node		heap_peek(t_heap *heap);

void			heap_extract_min(t_heap *heap);

void			heap_remove(t_heap *heap, int coder_num);

struct timespec	get_timespec_from_ms(long long ms);

void			take_dongles(int dongle_id, t_coder *coder);

void			release_dongles(int left_dongle_id, int right_dongle_id, t_coder *coder);

void			cleanup_sim(t_simulation *sim, pthread_t *th,
					bool destroy_mutexes);

bool			initialize_all_mutexes(t_simulation *sim);

long long		get_time_since_start(t_simulation *sim);

void			print_action(t_simulation *sim, int coder_id, char *action);

int				should_stop(t_simulation *sim);

void			custom_usleep(long long wait_time, t_simulation *sim);

void			*run_simulation(void *arg);

void			*run_monitor(void *arg);

#endif