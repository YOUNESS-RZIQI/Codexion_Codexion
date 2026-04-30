#ifndef SIMULATION_H
# define SIMULATION_H

# include "codexion.h"

void	put_dongles(t_coder *coder)
{
	release_dongle(coder->left_dongle, coder);
	release_dongle(coder->right_dongle, coder);
}

void	coder_sleep_and_think(t_simulation *sim, t_coder *coder)
{
	print_action(sim, coder->coder_number, "is debugging");
	custom_usleep(sim->args.time_to_debug, sim);
	print_action(sim, coder->coder_number, "is refactoring");
	custom_usleep(sim->args.time_to_refactor, sim);
}

int	check_compile_count(t_simulation *sim, t_coder *coder)
{
	pthread_mutex_lock(&sim->sim_mutex);
	coder->compile_count++;
	if (sim->args.number_of_compiles_required > 0
		&& coder->compile_count >= sim->args.number_of_compiles_required)
	{
		pthread_mutex_unlock(&sim->sim_mutex);
		return (1);
	}
	pthread_mutex_unlock(&sim->sim_mutex);
	coder_sleep_and_think(sim, coder);
	return (0);
}

int	execute_coder_cycle(t_simulation *sim, t_coder *coder)
{
	grab_dongles(coder);
	if (should_stop(sim))
	{
		put_dongles(coder);
		return (1);
	}
	pthread_mutex_lock(&sim->sim_mutex);
	coder->time_since_last_compile = get_current_time_ms();
	coder->deadline = coder->time_since_last_compile + coder->time_to_burnout;
	pthread_mutex_unlock(&sim->sim_mutex);
	print_action(sim, coder->coder_number, "is compiling");
	custom_usleep(sim->args.time_to_compile, sim);
	put_dongles(coder);
	return (check_compile_count(sim, coder));
}

void	*run_simulation(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	wait_at_barrier(sim);
	if (coder->coder_number % 2 == 0)
		custom_usleep(1, sim);
	while (!should_stop(sim))
	{
		if (execute_coder_cycle(sim, coder))
			break ;
	}
	return (NULL);
}

#endif
