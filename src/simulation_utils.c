/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:56 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:57 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_since_start(t_simulation *sim)
{
	return (get_current_time_ms() - sim->start_time);
}

void	print_action(t_simulation *sim, int coder_id, char *action)
{
	if (!should_stop(sim))
	{
		pthread_mutex_lock(&sim->sim_print_mutex);
		printf("%lld %d %s\n", get_time_since_start(sim), coder_id, action);
		pthread_mutex_unlock(&sim->sim_print_mutex);
	}
}

int	should_stop(t_simulation *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->sim_mutex);
	stop = sim->stop_simulation;
	pthread_mutex_unlock(&sim->sim_mutex);
	return (stop);
}

void	custom_usleep(long long wait_time, t_simulation *sim)
{
	long long	start;
	long long	now;
	long long	remaining;

	start = get_current_time_ms();

	while (1)
	{
		if (should_stop(sim))
			break ;

		now = get_current_time_ms();
		remaining = wait_time - (now - start);

		if (remaining <= 0)
			break ;

		if (remaining > 50)
			usleep(5000);
		else if (remaining > 10)
			usleep(1000);
		else if (remaining > 3)
			usleep(200);
	}
}
