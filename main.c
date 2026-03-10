/*
** main.c
** ============================================================
** WHAT  : Program entry point. Parses arguments, allocates
**         the simulation context, runs it, then cleans up.
** WHY   : main() is intentionally thin — all logic is in
**         other files.  This makes each file testable
**         independently.
** REMOVE: The program has no entry point and cannot run.
** ============================================================
**
** VISUAL — Program flow:
**
**   main()
**     │
**     ├── parse_args()        → validate 8 CLI arguments
**     │
**     ├── malloc(t_sim)       → allocate simulation context
**     │
**     ├── init mutexes        → log_mutex, monitor_mutex
**     │
**     ├── init_coders()       → allocate & assign dongles
**     │
**     ├── init_dongles()      → allocate & init each dongle
**     │
**     ├── run_simulation()    → launch threads, wait for end
**     │
**     ├── cleanup_sim()       → free all memory
**     │
**     └── return 0
*/

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_sim	*sim;

	/* ── Step 1: Parse and validate arguments ─────────────── */
	/*
	** We allocate sim on the heap (not stack) so its address
	** can be safely shared with threads.
	*/
	sim = (t_sim *)malloc(sizeof(t_sim));
	if (!sim)
	{
		fprintf(stderr, "Error: malloc failed\n");
		return (1);
	}
	memset(sim, 0, sizeof(t_sim)); /* zero all fields */

	if (!parse_args(argc, argv, &sim->args))
	{
		free(sim);
		return (1);
	}

	/* ── Step 2: Initialise shared mutexes ────────────────── */
	/*
	** log_mutex     → serialises all printf calls
	** monitor_mutex → protects the sim->stop flag
	** Both must exist before any thread is created.
	*/
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0
		|| pthread_mutex_init(&sim->monitor_mutex, NULL) != 0)
	{
		fprintf(stderr, "Error: mutex init failed\n");
		free(sim);
		return (1);
	}

	sim->stop       = 0;   /* simulation starts running */
	sim->start_time = 0;   /* set properly in run_simulation() */

	/* ── Step 3: Allocate coders ──────────────────────────── */
	/*
	** init_coders fills sim->coders[] with ids and dongle
	** indices.  Must happen before init_dongles (coders
	** reference dongles by index).
	*/
	if (!init_coders(sim))
	{
		fprintf(stderr, "Error: coder allocation failed\n");
		cleanup_sim(sim);
		free(sim);
		return (1);
	}

	/* ── Step 4: Allocate dongles ─────────────────────────── */
	if (!init_dongles(sim))
	{
		fprintf(stderr, "Error: dongle allocation failed\n");
		cleanup_sim(sim);
		free(sim);
		return (1);
	}

	/* ── Step 5: Run simulation ───────────────────────────── */
	/*
	** This blocks until all threads finish (either burnout or
	** everyone compiled enough times).
	*/
	if (!run_simulation(sim))
	{
		fprintf(stderr, "Error: simulation failed to start\n");
		cleanup_sim(sim);
		free(sim);
		return (1);
	}

	/* ── Step 6: Clean up and exit ────────────────────────── */
	cleanup_sim(sim);
	free(sim);
	return (0);
}
