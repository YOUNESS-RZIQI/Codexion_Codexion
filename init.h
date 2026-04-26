#ifndef INIT_H
#define INIT_H

#include "codexion.h"

static void	cleanup_partial_mutexes(t_simulation *sim, int step, int d_idx)
{
	int	i;

	if (step >= 1)
		pthread_mutex_destroy(&sim->print_mutex);
	if (step >= 2)
		pthread_mutex_destroy(&sim->sim_mutex);
	if (step >= 3)
	{
		pthread_cond_destroy(&sim->cond_lock);
		i = -1;
		while (++i < d_idx)
		{
			pthread_mutex_destroy(&sim->dongles[i].lock);
			pthread_cond_destroy(&sim->dongles[i].cond);
		}
	}
}

static short	init_dongle_mutexes(t_simulation *sim)
{
	int	i;

	i = -1;
	while (++i < sim->args.number_of_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].lock, NULL) != 0)
		{
			cleanup_partial_mutexes(sim, 3, i);
			return (1);
		}
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&sim->dongles[i].lock);
			cleanup_partial_mutexes(sim, 3, i);
			return (1);
		}
	}
	return (0);
}

short	initialize_all_mutexes(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->sim_mutex, NULL) != 0)
	{
		cleanup_partial_mutexes(sim, 1, 0);
		return (1);
	}
	if (pthread_cond_init(&sim->cond_lock, NULL) != 0)
	{
		cleanup_partial_mutexes(sim, 2, 0);
		return (1);
	}
	return (init_dongle_mutexes(sim));
}

void	init_dongles(t_simulation *sim)
{
	int	i;
	int	n;

	i = 0;
	n = sim->args.number_of_coders;
	while (i < n)
	{
		sim->dongles[i].number = i + 1;
		sim->dongles[i].dongle_is_available = 1;
		sim->dongles[i].how_much_to_rest = sim->args.dongle_cooldown;
		sim->dongles[i].toked_at = 0;
		sim->dongles[i].toked_by = 0;
		sim->dongles[i].queue_size = 0;
		if (i + 1 == 1)
		{
			sim->dongles[i].left_coder = n;
			sim->dongles[i].right_coder = 1;
		}
		else
		{
			sim->dongles[i].left_coder = i;
			sim->dongles[i].right_coder = i + 1;
		}
		i++;
	}
}

void	init_coders(t_simulation *sim)
{
	memset(sim->coders, 0, sizeof(t_coder) * sim->args.number_of_coders);
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		sim->coders[i].coder_number = i + 1;
		sim->coders[i].time_to_burnout = sim->args.time_to_burnout;
		sim->coders[i].compile_count = 0;
		i++;
	}
}

#endif