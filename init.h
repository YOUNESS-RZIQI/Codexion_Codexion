#ifndef INIT_H
# define INIT_H

# include "codexion.h"

void	destroy_dongle_mutexes(t_simulation *sim)
{
	int	i;

	i = -1;
	while (++i < sim->args.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].dongle_mutex);
		pthread_cond_destroy(&sim->dongles[i].dongle_cond);
	}
}

void	cleanup_sim(t_simulation *sim, pthread_t *th, short destroy_mutexes)
{
	if (sim->dongles)
	{
		if (destroy_mutexes)
			destroy_dongle_mutexes(sim);
		free(sim->dongles);
	}
	if (sim->coders)
		free(sim->coders);
	if (th)
		free(th);
	if (destroy_mutexes)
	{
		pthread_mutex_destroy(&sim->sim_print_mutex);
		pthread_mutex_destroy(&sim->sim_mutex);
		pthread_cond_destroy(&sim->sim_cond);
	}
}

void	cleanup_partial_mutexes(t_simulation *sim, int level, int dongle_idx)
{
	int	i;

	if (level >= 1)
		pthread_mutex_destroy(&sim->sim_print_mutex);
	if (level >= 2)
		pthread_mutex_destroy(&sim->sim_mutex);
	if (level >= 3)
		pthread_cond_destroy(&sim->sim_cond);
	i = 0;
	while (i < dongle_idx)
	{
		pthread_mutex_destroy(&sim->dongles[i].dongle_mutex);
		pthread_cond_destroy(&sim->dongles[i].dongle_cond);
		i++;
	}
}

short	init_dongle_mutexes(t_simulation *sim)
{
	int	i;

	i = -1;
	while (++i < sim->args.number_of_coders)
	{
		sim->dongles[i].heap.size = 0;
		if (pthread_mutex_init(&sim->dongles[i].dongle_mutex, NULL))
			break ;
		if (pthread_cond_init(&sim->dongles[i].dongle_cond, NULL))
		{
			pthread_mutex_destroy(&sim->dongles[i].dongle_mutex);
			break ;
		}
	}
	if (i == sim->args.number_of_coders)
		return (0);
	cleanup_partial_mutexes(sim, 3, i);
	return (1);
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

#endif
