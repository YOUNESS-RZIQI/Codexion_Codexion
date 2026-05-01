/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:30 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:31 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

struct timespec	get_timespec_from_ms(long long ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}

void	set_priority_and_insert(t_simulation *sim, t_dongle *dongle,
			t_coder *coder, t_heap_node *req)
{
	if (sim->args.scheduler_type == FIFO)
		req->priority = coder->coder_number;
	else
		req->priority = coder->deadline;
	heap_insert(&dongle->heap, *req, sim->args.scheduler_type);
}

int	check_take_dongle(t_dongle *d, t_coder *c, long long n)
{
	struct timespec	ts;

	if (d->dongle_is_available && n >= d->cooldown_end_time
		&& heap_peek(&d->heap).coder_number == c->coder_number)
	{
		heap_extract_min(&d->heap);
		d->dongle_is_available = 0;
		return (1);
	}
	if (d->dongle_is_available && n < d->cooldown_end_time
		&& heap_peek(&d->heap).coder_number == c->coder_number)
	{
		ts = get_timespec_from_ms(d->cooldown_end_time);
		pthread_cond_timedwait(&d->dongle_cond, &d->dongle_mutex, &ts);
	}
	else
		pthread_cond_wait(&d->dongle_cond, &d->dongle_mutex);
	return (0);
}

void	take_dongle(int dongle_id, t_coder *coder)
{
	t_simulation	*sim;
	t_dongle		*dongle;
	t_heap_node		req;
	long long		now;
	int				stop;

	sim = coder->sim;
	dongle = &sim->dongles[dongle_id];
	req.coder_number = coder->coder_number;
	pthread_mutex_lock(&dongle->dongle_mutex);
	set_priority_and_insert(sim, dongle, coder, &req);
	while (1)
	{
		now = get_current_time_ms();
		pthread_mutex_lock(&sim->sim_mutex);
		stop = sim->stop_simulation;
		pthread_mutex_unlock(&sim->sim_mutex);
		if (stop)
			break ;
		if (check_take_dongle(dongle, coder, now))
			break ;
	}
	pthread_mutex_unlock(&dongle->dongle_mutex);
}

void	release_dongle(int dongle_id, t_coder *coder)
{
	t_simulation	*sim;
	t_dongle		*dongle;
	long long		cld;

	sim = coder->sim;
	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->dongle_mutex);
	dongle->dongle_is_available = 1;
	cld = get_current_time_ms() + sim->args.dongle_cooldown;
	dongle->cooldown_end_time = cld;
	pthread_cond_broadcast(&dongle->dongle_cond);
	pthread_mutex_unlock(&dongle->dongle_mutex);
}
