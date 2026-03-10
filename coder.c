/*
** coder.c
** ============================================================
** WHAT  : The thread routine for each coder.
** WHY   : Subject: "Each coder must be represented by a thread
**         (using pthread_create)."
**         The lifecycle is: compile → debug → refactor → repeat
** REMOVE: No coders would exist; the simulation would do nothing.
** ============================================================
**
** VISUAL — Coder lifecycle (state machine):
**
**   START
**     │
**     ▼
**   ┌─────────────────────────────────────────────────────┐
**   │  WAIT FOR LEFT DONGLE (take_dongle left)            │
**   │  WAIT FOR RIGHT DONGLE (take_dongle right)          │
**   │  (uses FIFO or EDF priority queue)                  │
**   └───────────────────────┬─────────────────────────────┘
**                           │ both dongles acquired
**                           ▼
**   ┌─────────────────────────────────────────────────────┐
**   │  COMPILING  (hold both dongles, sleep time_to_compile│
**   └───────────────────────┬─────────────────────────────┘
**                           │ done
**                           ▼
**   ┌─────────────────────────────────────────────────────┐
**   │  release LEFT dongle (cooldown starts)              │
**   │  release RIGHT dongle (cooldown starts)             │
**   └───────────────────────┬─────────────────────────────┘
**                           │
**                           ▼
**   ┌─────────────────────────────────────────────────────┐
**   │  DEBUGGING  (sleep time_to_debug)                   │
**   └───────────────────────┬─────────────────────────────┘
**                           │ done
**                           ▼
**   ┌─────────────────────────────────────────────────────┐
**   │  REFACTORING (sleep time_to_refactor)               │
**   └───────────────────────┬─────────────────────────────┘
**                           │ done → go back to top
**                           ▼
**   Loop until sim->stop == 1
**
** DEADLOCK PREVENTION:
**   Using resource hierarchy: every coder always takes the
**   lower-indexed dongle FIRST, then the higher-indexed one.
**   This breaks the circular wait condition (Coffman #4).
**
**   Example (4 coders, 4 dongles):
**     Coder 1: left=0, right=1  → takes 0 then 1
**     Coder 2: left=1, right=2  → takes 1 then 2
**     Coder 3: left=2, right=3  → takes 2 then 3
**     Coder 4: left=3, right=0  → takes 0 then 3 (NOT 3 then 0!)
**
**   Without hierarchy: Coder 4 takes dongle 3, Coder 1 takes
**   dongle 0 — both wait for each other forever → DEADLOCK.
*/

#include "codexion.h"

/*
** is_stopped
** ──────────
** WHAT : Thread-safe check of the simulation stop flag.
** WHY  : Multiple threads read sim->stop; the monitor_mutex
**        ensures visibility of writes made by the monitor thread.
** REMOVE: A coder thread might miss the stop signal and
**         continue running, causing use-after-free or hangs.
*/
static int	is_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->monitor_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->monitor_mutex);
	return (stopped);
}

/*
** coder_routine
** ─────────────
** WHAT : Entry point for each coder thread (passed to
**        pthread_create).
** WHY  : Each coder independently loops through compile →
**        debug → refactor until the simulation stops.
** ARG  : void* cast of (t_coder *).
*/
void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;
	int		first;       /* lower-indexed dongle (taken first)  */
	int		second;      /* higher-indexed dongle (taken second)*/

	coder = (t_coder *)arg;
	sim   = coder->sim;

	/*
	** Determine dongle acquisition order to prevent deadlock.
	** Rule: always lock smaller index first.
	**
	** Coder sits between left and right dongle:
	**   coder->left  = (coder->id - 1) % N  (left neighbor)
	**   coder->right = coder->id % N         (right neighbor)
	*/
	if (coder->left < coder->right)
	{
		first  = coder->left;
		second = coder->right;
	}
	else
	{
		/* Last coder: right dongle has lower index than left */
		first  = coder->right;
		second = coder->left;
	}

	/* Special case: only one coder, only one dongle */
	if (sim->args.number_of_coders == 1)
	{
		/*
		** With 1 coder and 1 dongle the subject says there
		** is only ONE dongle on the table.  The coder needs
		** TWO to compile → they can never compile → they
		** burn out immediately.  We let the monitor detect
		** this naturally.
		*/
		return (NULL);
	}

	/*
	** Record initial last_compile_start so the burnout deadline
	** is counted from simulation start, not from first compile.
	*/
	coder->last_compile_start = sim->start_time;

	/* ── Main coder loop ──────────────────────────────────── */
	while (!is_stopped(sim))
	{
		/* ── Phase 1: Acquire both dongles ───────────────── */

		/* Take the lower-indexed dongle first (deadlock prevention) */
		take_dongle(sim, coder, first);
		if (is_stopped(sim))
			break ;

		/* Take the higher-indexed dongle second */
		take_dongle(sim, coder, second);
		if (is_stopped(sim))
		{
			/* We hold 'first' but not 'second'; release first */
			release_dongle(sim, first);
			break ;
		}

		/* ── Phase 2: Compiling ───────────────────────────── */

		/* Record when this compile started (used for EDF deadline
		   and for burnout detection by the monitor thread)         */
		coder->last_compile_start = now_ms();

		log_event(sim, coder->id, "is compiling");

		sleep_ms(sim->args.time_to_compile);

		/* Increment compile counter */
		coder->compile_count++;

		/* ── Phase 3: Release both dongles ───────────────── */

		/*
		** Release order does not matter for correctness, but
		** releasing immediately after compiling ensures other
		** coders can start acquiring as soon as possible.
		*/
		release_dongle(sim, first);
		release_dongle(sim, second);

		if (is_stopped(sim))
			break ;

		/* ── Phase 4: Debugging ───────────────────────────── */
		log_event(sim, coder->id, "is debugging");
		sleep_ms(sim->args.time_to_debug);

		if (is_stopped(sim))
			break ;

		/* ── Phase 5: Refactoring ─────────────────────────── */
		log_event(sim, coder->id, "is refactoring");
		sleep_ms(sim->args.time_to_refactor);

		/* After refactoring, loop back immediately to try to
		   acquire dongles again (subject: "immediately attempt
		   to acquire dongles and start compiling again")          */
	}

	return (NULL);
}
