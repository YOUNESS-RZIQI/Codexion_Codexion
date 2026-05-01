/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:53 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:54 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_all_compiled(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (sim->coders[i].compile_count
			< sim->args.number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

int	check_coders_burnout(t_simulation *sim)
{
	int			i;
	long long	now;
	long long	diff;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		now = get_current_time_ms();
		diff = now - sim->coders[i].time_since_last_compile;
		if (diff >= sim->coders[i].time_to_burnout)
		{
			sim->stop_simulation = 1;
			pthread_mutex_unlock(&sim->sim_mutex);
			pthread_mutex_lock(&sim->sim_print_mutex);
			printf("%lld %d burned out\n", get_time_since_start(sim),
				sim->coders[i].coder_number);
			pthread_mutex_unlock(&sim->sim_print_mutex);
			wake_all_dongles(sim);
			return (1);
		}
		i++;
	}
	return (0);
}

void	*run_monitor(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	wait_at_barrier(sim);
	while (1)
	{
		pthread_mutex_lock(&sim->sim_mutex);
		if (check_all_compiled(sim))
		{
			sim->stop_simulation = 1;
			pthread_mutex_unlock(&sim->sim_mutex);
			wake_all_dongles(sim);
			break ;
		}
		if (check_coders_burnout(sim))
			return (NULL);
		pthread_mutex_unlock(&sim->sim_mutex);
	}
	return (NULL);
}
