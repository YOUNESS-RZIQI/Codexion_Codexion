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

void take_dongles(int dongle_id, t_coder *c)
{
    (void)dongle_id;
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
    heap_insert(sim, &sim->dongles[first].heap, heap_node, sim->args.scheduler_type);
    heap_insert(sim, &sim->dongles[second].heap, heap_node, sim->args.scheduler_type);
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
            heap_extract_min(&sim->dongles[first].heap);
            heap_extract_min(&sim->dongles[second].heap);
            
            pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
            pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);
            return;
        }

        pthread_mutex_unlock(&sim->dongles[second].dongle_mutex);
        pthread_mutex_unlock(&sim->dongles[first].dongle_mutex);
        usleep(350);
    }

}

// void	release_dongles(int left_dongle_id, int right_dongle_id, t_coder *coder)
// {
// 	t_dongle *l_d;
// 	t_dongle *r_d;
//     long    long    now;

//     pthread_mutex_lock(&coder->sim->dongles[left_dongle_id].dongle_mutex);
//     pthread_mutex_lock(&coder->sim->dongles[right_dongle_id].dongle_mutex);

//     l_d = &coder->sim->dongles[left_dongle_id];
//     r_d = &coder->sim->dongles[right_dongle_id];
//     now = get_current_time_ms();

    
//     l_d->dongle_is_available = 1;
//     r_d->dongle_is_available = 1;

//     l_d->cooldown_end_time = now
//     + coder->sim->args.dongle_cooldown;

//     r_d->cooldown_end_time = now
//     + coder->sim->args.dongle_cooldown;

//     pthread_mutex_unlock(&coder->sim->dongles[right_dongle_id].dongle_mutex);
//     pthread_mutex_unlock(&coder->sim->dongles[left_dongle_id].dongle_mutex);
// }

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

    // Lock the REAL mutexes (via pointers), before reading/writing
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