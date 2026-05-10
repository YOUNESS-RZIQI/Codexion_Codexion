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


int is_dongle_ready(t_dongle *d, long long now)
{
    return (d->dongle_is_available && now >= d->cooldown_end_time);
}

int top_coder_can_take_both(t_simulation *sim, t_dongle *d, long long now)
{
    int coder_num;
    t_coder *top_c;
    t_dongle *l;
    t_dongle *r;

    if (d->heap.size == 0)
        return (0);
    coder_num = d->heap.waiters[0].coder_number;
    top_c = &sim->coders[coder_num - 1];
    l = &sim->dongles[top_c->left_dongle];
    r = &sim->dongles[top_c->right_dongle];
    return (is_dongle_ready(l, now) && is_dongle_ready(r, now));
}

int can_take_both(t_coder *c, long long now)
{
    t_simulation *sim = c->sim;
    t_dongle *l = &sim->dongles[c->left_dongle];
    t_dongle *r = &sim->dongles[c->right_dongle];

    if (!is_dongle_ready(l, now) || !is_dongle_ready(r, now))
        return (0);

    if (l->heap.size == 0 || l->heap.waiters[0].coder_number != c->coder_number)
        return (0);

    if (r->heap.size == 0 || r->heap.waiters[0].coder_number != c->coder_number)
        return (0);

    return (1);
}

void take_dongles(t_coder *c)
{
    t_simulation *sim = c->sim;
    t_heap_node heap_node;
    int     first;
    int     second;

    first = c->left_dongle;
    second = c->right_dongle;
    
    if ( first > second)
    {
        first = c->right_dongle;
        second = c->left_dongle;
    }
    
    heap_node.coder_number = c->coder_number;
    heap_node.priority = c->deadline;
    heap_node.compile_count = c->compile_count;

    pthread_mutex_lock(&sim->dongles[first].dongle_mutex);
    pthread_mutex_lock(&sim->dongles[second].dongle_mutex);
    heap_insert(sim, &sim->dongles[first].heap, heap_node);
    heap_insert(sim, &sim->dongles[second].heap, heap_node);
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
            sim->dongles[first].dongle_is_available = 0;
            sim->dongles[second].dongle_is_available = 0;
            heap_extract_min(sim, &sim->dongles[first].heap);
            heap_extract_min(sim, &sim->dongles[second].heap);
            
            pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
            pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);
            return;
        }

        int wait_on_first = 1;
        long long now_ms = get_current_time_ms();
        if (is_dongle_ready(&sim->dongles[first], now_ms) && 
            !is_dongle_ready(&sim->dongles[second], now_ms))
            wait_on_first = 0;

        t_dongle *d_to_wait = wait_on_first ? &sim->dongles[first] : &sim->dongles[second];
        long long target_time = now_ms + 10;
        
        if (d_to_wait->dongle_is_available && d_to_wait->cooldown_end_time > now_ms)
            target_time = d_to_wait->cooldown_end_time;

        if (wait_on_first)
        {
            pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
            struct timespec ts = get_timespec_from_ms(target_time);
            pthread_cond_timedwait(&sim->dongles[first].dongle_cond, &sim->dongles[first].dongle_mutex, &ts);
            pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);
        }
        else
        {
            pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);
            struct timespec ts = get_timespec_from_ms(target_time);
            pthread_cond_timedwait(&sim->dongles[second].dongle_cond, &sim->dongles[second].dongle_mutex, &ts);
            pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
        }
    }

    print_action(sim, c->coder_number, "has taken a dongle");
    print_action(sim, c->coder_number, "has taken a dongle");
}

void release_dongles(int left_dongle_id, int right_dongle_id, t_coder *coder)
{
    t_dongle    *l_d;
    t_dongle    *r_d;
    int         first;
    int         second;
    long long   now;

    first = left_dongle_id;
    second = right_dongle_id;
    
    if (first > second)
    {
        first = right_dongle_id;
        second = left_dongle_id;
    }

    l_d = &coder->sim->dongles[first];
    r_d = &coder->sim->dongles[second];

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