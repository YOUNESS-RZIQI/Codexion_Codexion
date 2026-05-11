/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:30 by yrziqi            #+#    #+#             */
/*   Updated: 2026/05/11 05:12:00 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	do_wait(t_dongle *d_wait, t_dongle *d_other, long long t_time)
{
	struct timespec	ts;

	ts = get_timespec_from_ms(t_time);
	pthread_mutex_unlock(&d_other->dongle_mutex);
	pthread_cond_timedwait(&d_wait->dongle_cond, &d_wait->dongle_mutex, &ts);
	pthread_mutex_unlock(&d_wait->dongle_mutex);
}

void	wait_for_dongles(t_simulation *s, int l_id, int r_id)
{
	int			w_first;
	long long	now;
	long long	t_time;
	t_dongle	*d_wait;

	now = get_current_time_ms();
	w_first = 1;
	if (is_dongle_ready(&s->dongles[l_id], now)
		&& !is_dongle_ready(&s->dongles[r_id], now))
		w_first = 0;
	if (w_first)
		d_wait = &s->dongles[l_id];
	else
		d_wait = &s->dongles[r_id];
	t_time = now + 10;
	if (d_wait->dongle_is_available && d_wait->cooldown_end_time > now)
		t_time = d_wait->cooldown_end_time;
	if (w_first)
		do_wait(d_wait, &s->dongles[r_id], t_time);
	else
		do_wait(d_wait, &s->dongles[l_id], t_time);
}

void	take_dongles(t_coder *c)
{
	int				l_id;
	int				r_id;
	t_simulation	*sim;

	sim = c->sim;
	l_id = c->left_dongle;
	r_id = c->right_dongle;
	enqueue_coder(c, l_id, r_id);
	while (1)
	{
		if (should_stop(sim))
			return ;
		pthread_mutex_lock(&sim->dongles[l_id].dongle_mutex);
		pthread_mutex_lock(&sim->dongles[r_id].dongle_mutex);
		if (can_take_both(c, get_current_time_ms()))
		{
			try_take(sim, l_id, r_id);
			return ;
		}
		wait_for_dongles(sim, l_id, r_id);
	}
	print_action(sim, c->coder_number, "has taken a dongle");
	print_action(sim, c->coder_number, "has taken a dongle");
}

void	release_dongles(int left_dongle_id, int right_dongle_id, t_coder *coder)
{
	t_dongle	*l_d;
	t_dongle	*r_d;
	long long	now;

	l_d = &coder->sim->dongles[left_dongle_id];
	r_d = &coder->sim->dongles[right_dongle_id];
	pthread_mutex_lock(&l_d->dongle_mutex);
	pthread_mutex_lock(&r_d->dongle_mutex);
	now = get_current_time_ms();
	l_d->dongle_is_available = 1;
	r_d->dongle_is_available = 1;
	l_d->cooldown_end_time = now + coder->sim->args.dongle_cooldown;
	r_d->cooldown_end_time = now + coder->sim->args.dongle_cooldown;
	pthread_mutex_unlock(&r_d->dongle_mutex);
	pthread_mutex_unlock(&l_d->dongle_mutex);
}
