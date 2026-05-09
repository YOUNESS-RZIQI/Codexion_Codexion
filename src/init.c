/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:41 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:42 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_dongle_mutexes_and_conds(t_simulation *sim)
{
	int	i;

	i = -1;
	while (++i < sim->args.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].dongle_mutex);
		pthread_cond_destroy(&sim->dongles[i].dongle_cond);
	}
}

void	cleanup_sim(t_simulation *sim, pthread_t *th, bool destroy_mutexes)
{
	if (sim->dongles)
	{
		if (destroy_mutexes)
			destroy_dongle_mutexes_and_conds(sim);
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

bool	init_dongle_mutexes_and_heap_size(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		sim->dongles[i].heap.size = 0;
		if (pthread_mutex_init(&sim->dongles[i].dongle_mutex, NULL))
			break ;
		if (pthread_cond_init(&sim->dongles[i].dongle_cond, NULL))
			break ;
		i++;
	}
	if (i == sim->args.number_of_coders)
		return (1);
	return (0);
}

bool	initialize_all_mutexes(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->sim_print_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->sim_mutex, NULL) != 0)
	{
		return (0);
	}
	if (pthread_cond_init(&sim->sim_cond, NULL) != 0)
	{
		return (0);
	}
	return (init_dongle_mutexes_and_heap_size(sim));
}
