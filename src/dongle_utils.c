/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 05:12:00 by yrziqi            #+#    #+#             */
/*   Updated: 2026/05/11 05:12:00 by yrziqi           ###   ########.fr       */
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

int	is_dongle_ready(t_dongle *d, long long now)
{
	return (d->dongle_is_available && now >= d->cooldown_end_time);
}

int	can_take_both(t_coder *c, long long now)
{
	t_simulation	*sim;
	t_dongle		*l;
	t_dongle		*r;

	sim = c->sim;
	l = &sim->dongles[c->left_dongle];
	r = &sim->dongles[c->right_dongle];
	if (!is_dongle_ready(l, now) || !is_dongle_ready(r, now))
		return (0);
	if (l->heap.size == 0
		|| l->heap.waiters[0].coder_number != c->coder_number)
		return (0);
	if (r->heap.size == 0
		|| r->heap.waiters[0].coder_number != c->coder_number)
		return (0);
	return (1);
}

void	enqueue_coder(t_coder *c, int l_id, int r_id)
{
	t_heap_node		h_node;
	t_simulation	*sim;

	sim = c->sim;
	h_node.coder_number = c->coder_number;
	h_node.priority = c->deadline;
	h_node.compile_count = c->compile_count;
	pthread_mutex_lock(&sim->dongles[l_id].dongle_mutex);
	pthread_mutex_lock(&sim->dongles[r_id].dongle_mutex);
	heap_insert(sim, &sim->dongles[l_id].heap, h_node);
	heap_insert(sim, &sim->dongles[r_id].heap, h_node);
	pthread_mutex_unlock(&sim->dongles[r_id].dongle_mutex);
	pthread_mutex_unlock(&sim->dongles[l_id].dongle_mutex);
}

void	try_take(t_simulation *sim, int l_id, int r_id)
{
	sim->dongles[l_id].dongle_is_available = 0;
	sim->dongles[r_id].dongle_is_available = 0;
	heap_extract_min(sim, &sim->dongles[l_id].heap);
	heap_extract_min(sim, &sim->dongles[r_id].heap);
	pthread_mutex_unlock(&sim->dongles[r_id].dongle_mutex);
	pthread_mutex_unlock(&sim->dongles[l_id].dongle_mutex);
}
