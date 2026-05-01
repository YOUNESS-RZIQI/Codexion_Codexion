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
	pthread_mutex_lock(&sim->sim_print_mutex);
	pthread_mutex_lock(&sim->sim_mutex);
	if (!sim->stop_simulation)
	{
		printf("%lld %d %s\n", get_time_since_start(sim), coder_id, action);
	}
	pthread_mutex_unlock(&sim->sim_mutex);
	pthread_mutex_unlock(&sim->sim_print_mutex);
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

	start = get_current_time_ms();
	while ((get_current_time_ms() - start) < wait_time)
	{
		if (should_stop(sim))
			break ;
		usleep(500);
	}
}

void	grab_dongles(t_coder *coder)
{
	take_dongle(coder->left_dongle, coder);
	print_action(coder->sim, coder->coder_number, "has taken a dongle");
	take_dongle(coder->right_dongle, coder);
	print_action(coder->sim, coder->coder_number, "has taken a dongle");
}
