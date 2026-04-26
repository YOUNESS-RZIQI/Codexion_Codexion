#ifndef SIMULATION_UTILS_H
# define SIMULATION_UTILS_H

# include "codexion.h"

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
	int	first;
	int	second;

	if (coder->left_dongle < coder->right_dongle)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	take_dongle(first, coder);
	print_action(coder->sim, coder->coder_number, "has taken a dongle");
	take_dongle(second, coder);
	print_action(coder->sim, coder->coder_number, "has taken a dongle");
}

#endif
