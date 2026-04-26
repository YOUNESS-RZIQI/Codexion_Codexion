#ifndef TIME_H
#define TIME_H

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

#endif
