/*
** monitor.c
** ============================================================
** WHAT  : A dedicated thread that watches for burnout and the
**         "all coders compiled enough" stop condition.
** WHY   : Subject: "A separate monitor thread must detect
**         burnout precisely and stop the simulation.  The
**         burnout log must be printed within 10 ms of the
**         actual burnout time."
** REMOVE: Burnout would never be detected (or detected too
**         late), and the simulation might never stop.
** ============================================================
**
** VISUAL — Monitor polling loop:
**
**   START
**     │
**     ▼  (every 1 ms)
**   ┌─────────────────────────────────────────────────────┐
**   │  FOR each coder i:                                  │
**   │    deadline = last_compile_start + time_to_burnout  │
**   │    if now_ms() > deadline → BURNED OUT!             │
**   │      → log "X burned out"                           │
**   │      → set sim->stop = 1                            │
**   │      → broadcast all dongle conds (wake blocked     │
**   │        threads so they can exit)                    │
**   │      → return                                       │
**   └─────────────────────────────────────────────────────┘
**     │
**     ▼
**   ┌─────────────────────────────────────────────────────┐
**   │  Have ALL coders compiled >= required times?        │
**   │    YES → set sim->stop = 1 → broadcast → return     │
**   └─────────────────────────────────────────────────────┘
**     │
**     └── sleep 1 ms → repeat
**
** WHY 1 ms polling: The subject allows 10 ms tolerance.
**   Polling every 1 ms guarantees we detect burnout within
**   1 ms of the deadline, well inside the 10 ms window.
*/

#include "codexion.h"

/*
** set_stop
** ────────
** WHAT : Atomically sets sim->stop = 1.
** WHY  : Multiple threads might try to set stop; the mutex
**        ensures only one write happens and others see it.
*/
static void	set_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->monitor_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->monitor_mutex);
}

/*
** wake_all_dongles
** ────────────────
** WHAT : Broadcasts on every dongle's condition variable.
** WHY  : When stop is set, coder threads may be sleeping
**        inside take_dongle's pthread_cond_wait.  We must
**        wake them so they can check is_stopped() and exit.
** REMOVE: Threads block forever → program never terminates.
*/
static void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

/*
** all_compiled_enough
** ───────────────────
** WHAT : Returns 1 if every coder has compiled at least
**        number_of_compiles_required times.
** WHY  : This is the "success" stop condition from the subject.
*/
static int	all_compiled_enough(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (sim->coders[i].compile_count
			< sim->args.number_of_compiles_required)
			return (0); /* at least one coder hasn't compiled enough */
		i++;
	}
	return (1); /* all coders are done */
}

/*
** monitor_routine
** ────────────────
** WHAT : Entry point for the monitor thread.
** WHY  : Runs independently, polling every 1 ms to check
**        burnout and completion conditions.
*/
void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;
	long	deadline;
	int		stopped;

	sim = (t_sim *)arg;

	while (1)
	{
		/* Sleep 1 ms between checks to minimise CPU usage
		   while staying within the 10 ms precision window */
		usleep(1000); /* 1000 µs = 1 ms */

		/* Check if stop was already set (e.g. by another check) */
		pthread_mutex_lock(&sim->monitor_mutex);
		stopped = sim->stop;
		pthread_mutex_unlock(&sim->monitor_mutex);
		if (stopped)
			return (NULL);

		/* ── Check 1: Did any coder burn out? ────────────── */
		i = 0;
		while (i < sim->args.number_of_coders)
		{
			/*
			** deadline = when the coder MUST start their next
			** compile.  If now > deadline → they burned out.
			**
			** last_compile_start starts as sim->start_time so
			** the very first compile is also monitored.
			*/
			deadline = sim->coders[i].last_compile_start
				+ sim->args.time_to_burnout;

			if (now_ms() > deadline)
			{
				/* Log burnout BEFORE setting stop so log_event
				   can still safely run */
				log_event(sim, sim->coders[i].id, "burned out");

				/* Signal everyone to stop */
				set_stop(sim);
				wake_all_dongles(sim);
				return (NULL);
			}
			i++;
		}

		/* ── Check 2: Have all coders compiled enough? ───── */
		if (all_compiled_enough(sim))
		{
			set_stop(sim);
			wake_all_dongles(sim);
			return (NULL);
		}
	}
}
