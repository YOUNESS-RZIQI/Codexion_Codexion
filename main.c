#include "codexion.h"

static void	cleanup_sim(t_simulation *sim, pthread_t *th, short destroy_mutexes)
{
	if (sim->dongles)
	{
		if (destroy_mutexes)
			destroy_dongle_mutexes(sim);
		free(sim->dongles);
	}
	if (sim->coders)
		free(sim->coders);
	if (th)
		free(th);
	if (destroy_mutexes)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		pthread_mutex_destroy(&sim->sim_mutex);
		pthread_cond_destroy(&sim->cond_lock);
	}
}

static void	handle_thread_creation_failure(t_simulation *sim, pthread_t *th,
	int created_coders, pthread_t *monitor)
{
	int	i;

	printf("handle_thread_creation_failure: setting stop_simulation\n");
	pthread_mutex_lock(&sim->sim_mutex);
	sim->stop_simulation = 1;
	pthread_cond_broadcast(&sim->cond_lock);
	pthread_mutex_unlock(&sim->sim_mutex);
	wake_all_dongles(sim);
	if (monitor)
		pthread_join(*monitor, NULL);
	i = -1;
	while (++i < created_coders)
		pthread_join(th[i], NULL);
}

static short	start_threads(t_simulation *sim, pthread_t *th)
{
	pthread_t	monitor;
	int			i;

	i = -1;
	while (++i < sim->args.number_of_coders)
	{
		if (pthread_create(&th[i], NULL, run_simulation, &sim->coders[i]) != 0)
		{
			handle_thread_creation_failure(sim, th, i, NULL);
			return (1);
		}
	}
	if (pthread_create(&monitor, NULL, run_monitor, sim) != 0)
	{
		handle_thread_creation_failure(sim, th, i, NULL);
		return (1);
	}
	pthread_join(monitor, NULL);
	i = -1;
	while (++i < sim->args.number_of_coders)
		pthread_join(th[i], NULL);
	return (0);
}

static short	initialize_simulation(t_simulation *sim, pthread_t **th)
{
	sim->coders = malloc(sizeof(t_coder) * sim->args.number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->args.number_of_coders);
	if (!sim->coders || !sim->dongles)
		return (1);
	*th = malloc(sizeof(pthread_t) * sim->args.number_of_coders);
	if (!*th)
		return (1);
	sim->stop_simulation = 0;
	sim->start_time = 0;
	sim->threads_at_barrier = 0;
	init_dongles(sim);
	init_coders(sim);
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
	if (initialize_simulation(&sim, &th) != 0)
	{
		cleanup_sim(&sim, th, 0);
		return (initialize_simulation_error_message());
	}
	if (initialize_all_mutexes(&sim) == 0)
	{
		start_threads(&sim, th);
		cleanup_sim(&sim, th, 1);
	}
	else
	{
		cleanup_sim(&sim, th, 0);
		return (mutex_error_message());
	}
	return (0);
}

/*

	args.c					|Not done|
	codexion.h				|Not done|
	init.c					|Not done|
	main.c					|Not done|
	utils.c					|Not done|
	dongle_utils.c			|Not done|
	utils_2.c				|Not done|
	heap.c					|Not done|
	dongle.c				|Not done|
	Makefile				|Not done|
	monitor.c				|Not done|
	simulation_utils.c		|Not done|
	simulation.c			|Not done|

*/