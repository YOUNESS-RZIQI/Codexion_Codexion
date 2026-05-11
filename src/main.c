/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:48 by yrziqi            #+#    #+#             */
/*   Updated: 2026/05/11 05:15:00 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	handle_thread_creation_failure(t_simulation *sim, pthread_t *th,
	int created_coders)
{
	int	i;

	pthread_mutex_lock(&sim->sim_mutex);
	sim->stop_simulation = 1;
	pthread_cond_broadcast(&sim->sim_cond);
	pthread_mutex_unlock(&sim->sim_mutex);
	i = -1;
	while (++i < created_coders)
		pthread_join(th[i], NULL);
}

bool	start_threads(t_simulation *sim, pthread_t *th)
{
	pthread_t	monitor;
	int			i;

	i = 0;
	if (pthread_create(&monitor, NULL, run_monitor, sim) != 0)
	{
		handle_thread_creation_failure(sim, th, i);
		return (1);
	}
	while (i < sim->args.number_of_coders)
	{
		if (pthread_create(&th[i], NULL, run_simulation, &sim->coders[i]) != 0)
		{
			handle_thread_creation_failure(sim, th, i);
			return (1);
		}
		i++;
	}
	wait_barrier_start(sim);
	pthread_join(monitor, NULL);
	i = -1;
	while (++i < sim->args.number_of_coders)
		pthread_join(th[i], NULL);
	return (0);
}

bool	initialize_simulation(t_simulation *sim, pthread_t **th)
{
	sim->coders = malloc(sizeof(t_coder) * sim->args.number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->args.number_of_coders);
	if (!sim->coders || !sim->dongles)
		return (1);
	*th = malloc(sizeof(pthread_t) * sim->args.number_of_coders);
	if (!*th)
		return (1);
	sim->stop_simulation = 0;
	sim->threads_at_barrier = 0;
	init_coders_and_dongles(sim);
	return (0);
}

bool	run_and_cleanup(t_simulation *sim, pthread_t *th)
{
	bool	result;

	if (initialize_all_mutexes(sim))
	{
		result = start_threads(sim, th);
		cleanup_sim(sim, th, 1);
		if (result)
			return (thread_creation_fail_error_message());
	}
	else
	{
		cleanup_sim(sim, th, 0);
		return (mutex_error_message());
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		*th;

	sim.args = convert_args(argc, argv);
	th = NULL;
	if (is_empty_args(sim.args))
		return (input_error_message());
	if (sim.args.number_of_coders == 0
		|| sim.args.number_of_compiles_required == 0)
		return (0);
	if (initialize_simulation(&sim, &th) != 0)
	{
		cleanup_sim(&sim, th, 0);
		return (initialize_simulation_error_message());
	}
	return (run_and_cleanup(&sim, th));
}
