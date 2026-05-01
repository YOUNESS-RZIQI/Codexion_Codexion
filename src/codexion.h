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

short			is_full_digit(char *s);

short			ft_is_space(char c);

short			ft_isdigit(char c);

long long		conversion(const char *s, short sign);

long long		ft_atoi(const char *s);

short			is_empty_args(t_args args);

t_args			empty_args(void);

short			check_args(char **argv);

t_args			args_converted(char **argv);

t_args			convert_args(int argc, char **argv);

short			null_error_message(void);

short			input_error_message(void);

short			initialize_simulation_error_message(void);

short			mutex_error_message(void);

short			thread_creation_fail_error_message(void);

long long		get_current_time_ms(void);

void			wake_all_dongles(t_simulation *sim);

void			wait_at_barrier(t_simulation *sim);

void			init_dongles(t_simulation *sim);

void			init_coders(t_simulation *sim);

int				compare_nodes(t_heap_node a, t_heap_node b, t_scheduler type);

void			swap_nodes(t_heap_node *a, t_heap_node *b);

void			heapify_up(t_heap *heap, int i, t_scheduler type);

void			heapify_down(t_heap *heap);

int				heap_is_empty(t_heap *heap);

int				heap_insert(t_heap *heap, t_heap_node node, t_scheduler type);

t_heap_node		heap_peek(t_heap *heap);

void			heap_extract_min(t_heap *heap);

struct timespec	get_timespec_from_ms(long long ms);

void			set_priority_and_insert(t_simulation *sim, t_dongle *dongle,
					t_coder *coder, t_heap_node *req);

int				check_take_dongle(t_dongle *d, t_coder *c, long long n);

void			take_dongle(int dongle_id, t_coder *coder);

void			release_dongle(int dongle_id, t_coder *coder);

void			destroy_dongle_mutexes_and_conds(t_simulation *sim);

void			cleanup_sim(t_simulation *sim, pthread_t *th,
					short destroy_mutexes);

short			init_dongle_mutexes_and_heap_size(t_simulation *sim);

short			initialize_all_mutexes(t_simulation *sim);

long long		get_time_since_start(t_simulation *sim);

void			print_action(t_simulation *sim, int coder_id, char *action);

int				should_stop(t_simulation *sim);

void			custom_usleep(long long wait_time, t_simulation *sim);

void			grab_dongles(t_coder *coder);

void			put_dongles(t_coder *coder);

void			coder_debugg_and_refactor(t_simulation *sim, t_coder *coder);

int				check_compile_count(t_simulation *sim, t_coder *coder);

int				execute_coder_cycle(t_simulation *sim, t_coder *coder);

void			*run_simulation(void *arg);

int				check_all_compiled(t_simulation *sim);

int				check_coders_burnout(t_simulation *sim);

void			*run_monitor(void *arg);

#endif