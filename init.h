#ifndef INIT_H
#define INIT_H

#include "codexion.h"


static short	init_dongle_mutexes(t_simulation *sim)
{
	int	i;

	i = -1;
	while (++i < sim->args.number_of_coders)
	{
		pthread_mutex_init(&sim->dongles[i].dongle_mutex, NULL);
		
		pthread_cond_init(&sim->dongles[i].dongle_cond, NULL);
	}
	return (0);
}

short	initialize_all_mutexes(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->sim_print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->sim_mutex, NULL) != 0)
	{
		cleanup_partial_mutexes(sim, 1, 0);
		return (1);
	}
	if (pthread_cond_init(&sim->sim_cond, NULL) != 0)
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