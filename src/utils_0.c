/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_0.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:04:01 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:04:02 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

long long	get_current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	wake_all_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].dongle_mutex);
		pthread_cond_broadcast(&sim->dongles[i].dongle_cond);
		pthread_mutex_unlock(&sim->dongles[i].dongle_mutex);
		i++;
	}
}

void	wait_at_barrier(t_simulation *sim)
{
	int	i;

	pthread_mutex_lock(&sim->sim_mutex);
	sim->threads_at_barrier++;
	if (sim->threads_at_barrier == sim->args.number_of_coders + 1)
	{
		sim->start_time = get_current_time_ms();
		i = 0;
		while (i < sim->args.number_of_coders)
		{
			sim->coders[i].time_since_last_compile = sim->start_time;
			sim->coders[i].deadline = sim->start_time
				+ sim->coders[i].time_to_burnout;
			i++;
		}
		pthread_cond_broadcast(&sim->sim_cond);
	}
	else
	{
		if (!sim->stop_simulation)
			pthread_cond_wait(&sim->sim_cond, &sim->sim_mutex);
	}
	pthread_mutex_unlock(&sim->sim_mutex);
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
		sim->dongles[i].cooldown_end_time = 0;
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
	int	i;
	int	n;

	memset(sim->coders, 0, sizeof(t_coder) * sim->args.number_of_coders);
	i = 0;
	n = sim->args.number_of_coders;
	while (i < n)
	{
		sim->coders[i].coder_number = i + 1;
		sim->coders[i].time_to_burnout = sim->args.time_to_burnout;
		sim->coders[i].compile_count = 0;
		sim->coders[i].time_since_last_compile = 0;
		sim->coders[i].sim = sim;
		if (i == 0)
			sim->coders[i].left_dongle = n - 1;
		else
			sim->coders[i].left_dongle = i - 1;
		sim->coders[i].right_dongle = i;
		i++;
	}
}
