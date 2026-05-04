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
	if (sim->args.scheduler_type == EDF)
		req->priority = coder->deadline;
	heap_insert(&dongle->heap, *req, sim->args.scheduler_type);
}


int can_take_both(t_coder *c, long long now)
{
    t_simulation *sim = c->sim;
    t_dongle *l = &sim->dongles[c->left_dongle];
    t_dongle *r = &sim->dongles[c->right_dongle];

    if (!l->dongle_is_available || !r->dongle_is_available)
        return 0;

    if (now < l->cooldown_end_time || now < r->cooldown_end_time)
        return 0;

    if (l->heap.size == 0 || heap_peek(&l->heap).coder_number != c->coder_number)
        return 0;

    if (r->heap.size == 0 || heap_peek(&r->heap).coder_number != c->coder_number)
        return 0;

    return 1;
}

void take_dongle(int dongle_id, t_coder *c)
{
    (void)dongle_id;
    t_simulation *sim = c->sim;
    t_heap_node req;
    int first, second;

    first = (c->left_dongle < c->right_dongle) ? c->left_dongle : c->right_dongle;
    second = (c->left_dongle < c->right_dongle) ? c->right_dongle : c->left_dongle;

    req.coder_number = c->coder_number;
    req.priority = (sim->args.scheduler_type == EDF) ? c->deadline : 0;

    // Register interest in both dongles
    pthread_mutex_lock(&sim->dongles[first].dongle_mutex);
    pthread_mutex_lock(&sim->dongles[second].dongle_mutex);
    heap_insert(&sim->dongles[c->left_dongle].heap, req, sim->args.scheduler_type);
    heap_insert(&sim->dongles[c->right_dongle].heap, req, sim->args.scheduler_type);
    pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
    pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);

    while (1)
    {
        if (should_stop(sim))
            return;

        pthread_mutex_lock(&sim->dongles[first].dongle_mutex);
        pthread_mutex_lock(&sim->dongles[second].dongle_mutex);

        if (can_take_both(c, get_current_time_ms()))
        {
            sim->dongles[c->left_dongle].dongle_is_available = 0;
            sim->dongles[c->right_dongle].dongle_is_available = 0;
            heap_extract_min(&sim->dongles[c->left_dongle].heap);
            heap_extract_min(&sim->dongles[c->right_dongle].heap);
            
            pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
            pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);
            return;
        }

        pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
        pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);
        usleep(200);
    }
}

void	release_dongle(int dongle_id, t_coder *coder)
{

	t_dongle *d = &coder->sim->dongles[dongle_id];

    pthread_mutex_lock(&d->dongle_mutex);

    d->dongle_is_available = 1;
    d->cooldown_end_time = get_current_time_ms()
        + coder->sim->args.dongle_cooldown;

    pthread_cond_broadcast(&d->dongle_cond);

    pthread_mutex_unlock(&d->dongle_mutex);
}
