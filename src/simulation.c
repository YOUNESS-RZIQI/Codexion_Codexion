/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:59 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:04:00 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

bool	coder_debugg_and_refactor(t_simulation *sim, t_coder *coder)
{
	if (should_stop(sim))
	{
		release_dongles(coder->left_dongle, coder->right_dongle, coder);
		return (1);
	}
	print_action(sim, coder->coder_number, "is debugging");
	custom_usleep(sim->args.time_to_debug, sim);

	if (should_stop(sim))
	{
		release_dongles(coder->left_dongle, coder->right_dongle, coder);
		return (1);
	}
	print_action(sim, coder->coder_number, "is refactoring");
	custom_usleep(sim->args.time_to_refactor, sim);
	return (0);
}

bool	execute_coder_cycle(t_simulation *sim, t_coder *coder)
{	
	take_dongles(0, coder);
	if (should_stop(sim))
	{
		release_dongles(coder->left_dongle, coder->right_dongle, coder);
		return (1);
	}
	pthread_mutex_lock(&sim->sim_mutex);
	coder->time_since_last_compile_start = get_current_time_ms();
	coder->deadline = coder->time_since_last_compile_start + coder->time_to_burnout;
	pthread_mutex_unlock(&sim->sim_mutex);
	if (!should_stop(sim))
	{
		pthread_mutex_lock(&sim->sim_print_mutex);
		printf("%lld %d %s\n", get_time_since_start(sim), coder->coder_number, "has taken a dongle");
		printf("%lld %d %s\n", get_time_since_start(sim), coder->coder_number, "has taken a dongle");
		printf("%lld %d %s\n", get_time_since_start(sim), coder->coder_number, "is compiling");
		pthread_mutex_unlock(&sim->sim_print_mutex);
	}
	custom_usleep(sim->args.time_to_compile, sim);
	release_dongles(coder->left_dongle, coder->right_dongle, coder);
	pthread_mutex_lock(&sim->sim_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&sim->sim_mutex);
	return (coder_debugg_and_refactor(sim, coder));
}

void	*run_simulation(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;

	pthread_mutex_lock(&sim->sim_mutex);
	coder->time_since_last_compile_start = get_current_time_ms();
	coder->deadline = coder->time_since_last_compile_start + coder->time_to_burnout;
	pthread_mutex_unlock(&sim->sim_mutex);
	if (wait_at_barrier(sim))
		return (NULL);
	pthread_mutex_lock(&sim->sim_mutex);
	coder->time_since_last_compile_start = sim->start_time;
	coder->deadline = coder->time_since_last_compile_start + coder->time_to_burnout;
	pthread_mutex_unlock(&sim->sim_mutex);
	if (coder->coder_number % 2 == 0)
		custom_usleep(sim->args.time_to_compile / 2, sim);
	while (!should_stop(sim))
	{
		if (execute_coder_cycle(sim, coder))
			break ;
	}
	return (NULL);
}
