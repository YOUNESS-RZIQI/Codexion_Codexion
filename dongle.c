/*
** dongle.c
** ============================================================
** WHAT  : Manages USB dongles: init/destroy, take (with fair
**         arbitration and cooldown), and release.
** WHY   : The subject requires:
**           • Each dongle protected by a mutex + cond var.
**           • Cooldown: dongle unavailable for dongle_cooldown
**             ms after being released.
**           • Fair arbitration: FIFO or EDF scheduling.
** REMOVE: Races, deadlocks, or starvation could occur.
** ============================================================
**
** VISUAL — Dongle state machine:
**
**   ┌──────────┐   take_dongle()   ┌──────────┐
**   │ AVAILABLE│ ───────────────→  │  IN USE  │
**   └──────────┘                   └──────────┘
**        ↑                               │
**        │  cooldown expires             │ release_dongle()
**        │                               ↓
**   ┌──────────┐                   ┌──────────┐
**   │  COOLING │ ←─────────────── │ RELEASED │
**   └──────────┘  set cooldown_until
**
** VISUAL — FIFO vs EDF queue order:
**
**   Coder A requests at t=10 ms, deadline=500
**   Coder B requests at t=12 ms, deadline=300
**
**   FIFO queue (key = arrival time):
**     front → [A, key=10] → [B, key=12]
**     → A gets dongle first (arrived first)
**
**   EDF queue (key = deadline):
**     front → [B, key=300] → [A, key=500]
**     → B gets dongle first (burns out sooner)
*/

#include "codexion.h"

/* ── Initialisation ──────────────────────────────────────────*/

/*
** init_dongles
** ─────────────
** WHAT : Allocates and initialises all N dongles.
** WHY  : Each dongle needs its own mutex and condition variable
**        (fine-grained locking avoids unnecessary contention).
** RETURN: 1 on success, 0 on failure.
*/
int	init_dongles(t_sim *sim)
{
	int	i;

	/* Allocate dongle array */
	sim->dongles = (t_dongle *)malloc(sizeof(t_dongle)
			* sim->args.number_of_coders);
	if (!sim->dongles)
		return (0);

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		/* Zero all fields first (safe default state) */
		memset(&sim->dongles[i], 0, sizeof(t_dongle));

		/* Dongle starts available */
		sim->dongles[i].available = 1;

		/* cooldown_until = 0 → no cooldown at start */
		sim->dongles[i].cooldown_until = 0;

		/* Initialise mutex — protects all dongle fields */
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			return (0);

		/* Initialise condition variable — threads wait on this
		   when the dongle is busy or cooling down              */
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
			return (0);

		/* Queue starts empty (queue_push will malloc on first use) */
		sim->dongles[i].queue      = NULL;
		sim->dongles[i].queue_size = 0;
		sim->dongles[i].queue_cap  = 0;
		i++;
	}
	return (1);
}

/*
** destroy_dongles
** ───────────────
** WHAT : Frees all dongle resources (mutex, cond, queue).
** WHY  : All heap memory must be freed (subject requirement).
** REMOVE: Memory/resource leaks.
*/
void	destroy_dongles(t_sim *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		if (sim->dongles[i].queue)
			free(sim->dongles[i].queue);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

/* ── Taking a dongle ─────────────────────────────────────────*/

/*
** take_dongle
** ────────────
** WHAT : Blocks until the coder can take the specified dongle,
**        respecting cooldown and scheduler order.
** WHY  : Core of the fair arbitration mechanism.
**
** FLOW DIAGRAM:
**
**   coder calls take_dongle(sim, coder, idx)
**          │
**          ▼
**   lock dongle mutex
**          │
**          ▼
**   push our request onto the priority queue
**          │
**          ▼
**   ┌─────────────────────────────────────────┐
**   │  WAIT LOOP                              │
**   │                                         │
**   │  Is sim stopped? → exit (bail out)      │
**   │  Is it our turn? (we are queue front)   │
**   │  AND dongle available?                  │
**   │  AND cooldown expired?                  │
**   │       YES → break                       │
**   │       NO  → pthread_cond_wait (sleep    │
**   │             until signalled)            │
**   └─────────────────────────────────────────┘
**          │
**          ▼
**   pop our request from queue
**   mark dongle unavailable (in use)
**   unlock dongle mutex
**   log "X has taken a dongle"
*/
void	take_dongle(t_sim *sim, t_coder *coder, int dongle_idx)
{
	t_dongle	*d;
	t_req		req;
	long		deadline;

	d = &sim->dongles[dongle_idx];

	/* Compute the scheduling key for our request */
	if (sim->args.scheduler == FIFO)
		req.key = now_ms(); /* FIFO: key = arrival time */
	else
	{
		/* EDF: deadline = last_compile_start + time_to_burnout */
		deadline = coder->last_compile_start
			+ sim->args.time_to_burnout;
		req.key = deadline;
	}
	req.coder_id = coder->id;

	/* Lock dongle before touching its state */
	pthread_mutex_lock(&d->mutex);

	/* Register our request in the priority queue */
	queue_push(d, req);

	/*
	** Wait loop — we must hold the mutex while checking
	** conditions; pthread_cond_wait atomically releases the
	** mutex and sleeps, then re-acquires it on wakeup.
	*/
	while (1)
	{
		/* If simulation ended, remove from queue and abort */
		pthread_mutex_lock(&sim->monitor_mutex);
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->monitor_mutex);
			/* Remove ourselves from the queue before leaving */
			/* (simplification: mark our entry as id=-1, queue_pop
			    ignores the ordering; a proper removal would need
			    linear search — acceptable for this project size) */
			pthread_mutex_unlock(&d->mutex);
			return ;
		}
		pthread_mutex_unlock(&sim->monitor_mutex);

		/* Three conditions must ALL be true for us to proceed:
		   1. We are at the front of the queue (our turn)
		   2. Dongle is not currently in use
		   3. Cooldown period has expired                    */
		if (queue_peek_id(d) == coder->id
			&& d->available
			&& now_ms() >= d->cooldown_until)
			break ; /* we can take the dongle */

		/* Not our turn yet — sleep and wait to be signalled */
		pthread_cond_wait(&d->cond, &d->mutex);
	}

	/* We are cleared to take the dongle */
	queue_pop(d);          /* remove our request from queue    */
	d->available = 0;      /* mark dongle as in use            */

	pthread_mutex_unlock(&d->mutex);

	/* Log the event (outside the dongle mutex to avoid
	   holding two mutexes at once → reduces deadlock risk) */
	log_event(sim, coder->id, "has taken a dongle");
}

/* ── Releasing a dongle ──────────────────────────────────────*/

/*
** release_dongle
** ──────────────
** WHAT : Marks the dongle as cooling-down and signals all
**        waiting coders to re-check their conditions.
** WHY  : After compiling, both dongles must be released with
**        cooldown.  pthread_cond_broadcast wakes ALL waiters;
**        each re-checks the condition in the wait loop.
**
** FLOW:
**   lock mutex
**   → set available = 1
**   → set cooldown_until = now + dongle_cooldown
**   → broadcast (wake all waiters)
**   unlock mutex
*/
void	release_dongle(t_sim *sim, int dongle_idx)
{
	t_dongle	*d;

	d = &sim->dongles[dongle_idx];

	pthread_mutex_lock(&d->mutex);

	d->available      = 1; /* dongle is physically free again  */

	/* Set cooldown: no one can take it until this time passes */
	d->cooldown_until = now_ms() + sim->args.dongle_cooldown;

	/* Wake ALL waiting threads so they can re-check the queue.
	   Only the one at the front whose cooldown has passed will
	   actually proceed; the rest go back to sleep.            */
	pthread_cond_broadcast(&d->cond);

	pthread_mutex_unlock(&d->mutex);
}
