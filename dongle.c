#include "codexion.h"

/*
** VISUAL — how the queue works:
**
**  FIFO: key = arrival time  → smallest key = waited longest
**  EDF:  key = deadline      → smallest key = burns out soonest
**
**  Both use the same array; we just pick who has the min key.
**
**  queue[] = [3, 1, 2]   (coder ids waiting)
**  keys[]  = [10, 5, 8]  (their keys)
**  → coder 1 goes next (key=5 is smallest)
*/

/* add coder to dongle's waiting list */
static void	enqueue(t_dongle *d, int coder_id, long key)
{
	d->queue[d->qsize] = coder_id;
	d->keys[d->qsize]  = key;
	d->qsize++;
}

/* who has the smallest key? returns their coder_id, -1 if empty */
static int	peek_front(t_dongle *d)
{
	int		best_i;
	int		i;

	if (d->qsize == 0)
		return (-1);
	best_i = 0;
	i = 1;
	while (i < d->qsize)
	{
		if (d->keys[i] < d->keys[best_i])
			best_i = i;
		i++;
	}
	return (d->queue[best_i]);
}

/* remove coder_id from the waiting list */
static void	dequeue(t_dongle *d, int coder_id)
{
	int	i;

	i = 0;
	while (i < d->qsize)
	{
		if (d->queue[i] == coder_id)
		{
			/* replace with last entry and shrink */
			d->queue[i] = d->queue[d->qsize - 1];
			d->keys[i]  = d->keys[d->qsize - 1];
			d->qsize--;
			return ;
		}
		i++;
	}
}

/*
** take_dongle — block until this coder can hold the dongle
**
**  FLOW:
**    lock dongle mutex
**    register in queue
**    loop:
**      if stopped → leave
**      if our turn AND not in_use AND cooldown passed → break
**      else → wait on cond (releases mutex until signalled)
**    mark dongle in_use, dequeue ourselves
**    unlock
**    log "has taken a dongle"
*/
void	take_dongle(t_sim *s, t_coder *c, int idx)
{
	t_dongle	*d;
	long		key;

	d = &s->dongles[idx];

	/* compute our scheduling key */
	if (s->use_edf)
		key = c->last_compile + s->t_burnout; /* deadline */
	else
		key = now_ms(); /* arrival time for FIFO */

	pthread_mutex_lock(&d->mutex);
	enqueue(d, c->id, key);

	while (1)
	{
		if (is_stopped(s))
		{
			dequeue(d, c->id);
			pthread_mutex_unlock(&d->mutex);
			return ;
		}
		/* three conditions: our turn + free + cooldown expired */
		if (peek_front(d) == c->id
			&& !d->in_use
			&& now_ms() >= d->cooldown_until)
			break ;
		pthread_cond_wait(&d->cond, &d->mutex);
	}
	dequeue(d, c->id);
	d->in_use = 1;
	pthread_mutex_unlock(&d->mutex);

	log_msg(s, c->id, "has taken a dongle");
}

/*
** drop_dongle — release dongle and start its cooldown
*/
void	drop_dongle(t_sim *s, int idx)
{
	t_dongle	*d;

	d = &s->dongles[idx];
	pthread_mutex_lock(&d->mutex);
	d->in_use        = 0;
	d->cooldown_until = now_ms() + s->cooldown;
	pthread_cond_broadcast(&d->cond); /* wake all waiters */
	pthread_mutex_unlock(&d->mutex);
}
