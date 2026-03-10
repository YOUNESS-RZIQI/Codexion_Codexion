/*
** sim.c
** ============================================================
** WHAT  : Allocates coders, assigns dongles, launches threads,
**         waits for them, and cleans everything up.
** WHY   : Separating setup/teardown from the thread logic keeps
**         main.c minimal and sim.c focused on lifecycle.
** REMOVE: Threads would never start; memory would leak.
** ============================================================
**
** VISUAL — Thread topology for N=4 coders:
**
**   Dongles:   [D0]  [D1]  [D2]  [D3]
**                │  ╲   │  ╲   │  ╲   │
**   Coders:  [C1]  [C2]  [C3]  [C4]
**               left right left right ...
**
**   C1 uses D0 (left) and D1 (right)
**   C2 uses D1 (left) and D2 (right)
**   C3 uses D2 (left) and D3 (right)
**   C4 uses D3 (left) and D0 (right)  ← wraps around (circular)
**
**   Thread IDs:
**     threads[0..N-1] → coder threads
**     threads[N]      → monitor thread
*/

#include "codexion.h"

/*
** init_coders
** ───────────
** WHAT : Allocates and fills the coders array.
** WHY  : Each coder needs its id, dongle indices, and a
**        back-pointer to the sim context.
** RETURN: 1 on success, 0 on malloc failure.
*/
int	init_coders(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->args.number_of_coders;
	sim->coders = (t_coder *)malloc(sizeof(t_coder) * n);
	if (!sim->coders)
		return (0);

	i = 0;
	while (i < n)
	{
		memset(&sim->coders[i], 0, sizeof(t_coder));

		/* 1-based id as required by the subject */
		sim->coders[i].id = i + 1;

		/*
		** Dongle layout (circular table):
		**   left  dongle of coder i = index i
		**   right dongle of coder i = index (i + 1) % N
		**
		** Example N=3:
		**   coder 1 (i=0): left=0, right=1
		**   coder 2 (i=1): left=1, right=2
		**   coder 3 (i=2): left=2, right=0  ← wraps
		*/
		sim->coders[i].left  = i;
		sim->coders[i].right = (i + 1) % n;

		/* last_compile_start = start_time so burnout is measured
		   from the very beginning of the simulation             */
		sim->coders[i].last_compile_start = sim->start_time;

		sim->coders[i].compile_count = 0;
		sim->coders[i].sim           = sim; /* back-pointer */

		i++;
	}
	return (1);
}

/*
** run_simulation
** ──────────────
** WHAT : Launches all coder threads + monitor thread, waits
**        for all of them to finish.
** WHY  : pthread_join ensures main() does not return while
**        threads are still running (would cause UB).
** RETURN: 1 on success, 0 if any pthread call fails.
*/
int	run_simulation(t_sim *sim)
{
	int			n;
	pthread_t	*threads;
	int			i;

	n = sim->args.number_of_coders;

	/* Allocate thread handles: N coders + 1 monitor */
	threads = (pthread_t *)malloc(sizeof(pthread_t) * (n + 1));
	if (!threads)
		return (0);

	/* Record the simulation start time used for log timestamps */
	sim->start_time = now_ms();

	/* Reset last_compile_start now that we have start_time */
	i = 0;
	while (i < n)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		i++;
	}

	/* Launch coder threads */
	i = 0;
	while (i < n)
	{
		if (pthread_create(&threads[i], NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			free(threads);
			return (0);
		}
		i++;
	}

	/* Launch monitor thread (index N) */
	if (pthread_create(&threads[n], NULL,
			monitor_routine, sim) != 0)
	{
		free(threads);
		return (0);
	}

	/* Wait for all threads to finish */
	i = 0;
	while (i <= n) /* n coder threads + 1 monitor = n+1 total */
	{
		pthread_join(threads[i], NULL);
		i++;
	}

	free(threads);
	return (1);
}

/*
** cleanup_sim
** ───────────
** WHAT : Frees all simulation resources.
** WHY  : The subject requires all heap memory to be freed.
** REMOVE: Memory leaks.
*/
void	cleanup_sim(t_sim *sim)
{
	/* Destroy dongles (mutex, cond, queue arrays) */
	destroy_dongles(sim);

	/* Free coders array */
	if (sim->coders)
	{
		free(sim->coders);
		sim->coders = NULL;
	}

	/* Destroy shared mutexes */
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->monitor_mutex);
}
