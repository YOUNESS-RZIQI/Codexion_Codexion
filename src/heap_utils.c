/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:36 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:37 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	should_swap(t_simulation *sim, t_heap_node shiled, t_heap_node parent)
{
	if (sim->args.scheduler_type == FIFO)
		return DONOTSWAP;
	if (shiled.priority < parent.priority)
		return (SWAP);
	if (shiled.priority > parent.priority)
		return (DONOTSWAP);
	if (shiled.compile_count < parent.compile_count)
		return (SWAP);
	if (shiled.compile_count > parent.compile_count)
		return (DONOTSWAP);
	if (shiled.coder_number < parent.coder_number)
		return (SWAP);
	if (shiled.coder_number > parent.coder_number)
		return (DONOTSWAP);
	return (DONOTSWAP);
}

void	swap_waiters(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void    heapify_up(t_simulation *sim, t_heap *heap)
{
    int child;
    int parent;

    child = heap->size - 1;

    while (child > 0)
    {
        parent = (child - 1) / 2;

        if (should_swap(sim, heap->waiters[child], heap->waiters[parent]) == DONOTSWAP)
            break;

        swap_waiters(&heap->waiters[parent], &heap->waiters[child]);

        child = parent;
    }
}

void	heapify_down(t_heap *heap)
{
    int		l_child;
    int 	r_child;
    int 	child_to_compare;
    int 	parent;

    while (child > 0)
    {
        parent = (child - 1) / 2;

        if (should_swap(sim, heap->waiters[child], heap->waiters[parent]) == DONOTSWAP)
            break;

        swap_waiters(&heap->waiters[parent], &heap->waiters[child]);

        child = parent;
    }

}

