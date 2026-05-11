/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:36 by yrziqi            #+#    #+#             */
/*   Updated: 2026/05/11 05:15:00 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	should_swap(t_simulation *sim, t_heap_node child, t_heap_node parent)
{
	if (sim->args.scheduler_type == FIFO)
		return (DONOTSWAP);
	if (child.priority < parent.priority)
		return (SWAP);
	if (child.priority > parent.priority)
		return (DONOTSWAP);
	if (child.compile_count < parent.compile_count)
		return (SWAP);
	if (child.compile_count > parent.compile_count)
		return (DONOTSWAP);
	if (child.coder_number < parent.coder_number)
		return (SWAP);
	if (child.coder_number > parent.coder_number)
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

void	heapify_up(t_simulation *sim, t_heap *heap)
{
	int	child;
	int	parent;

	child = heap->size - 1;
	while (child > 0)
	{
		parent = (child - 1) / 2;
		if (should_swap(sim, heap->waiters[child],
				heap->waiters[parent]) == DONOTSWAP)
			break ;
		swap_waiters(&heap->waiters[parent], &heap->waiters[child]);
		child = parent;
	}
}

void	heapify_down(t_simulation *sim, t_heap *heap)
{
	int	parent;
	int	l_child;
	int	r_child;
	int	sm_or_hi;

	parent = 0;
	while (1)
	{
		l_child = (2 * parent) + 1;
		r_child = (2 * parent) + 2;
		sm_or_hi = parent;
		if (l_child < heap->size && should_swap(sim, heap->waiters[l_child],
				heap->waiters[sm_or_hi]) == SWAP)
			sm_or_hi = l_child;
		if (r_child < heap->size && should_swap(sim, heap->waiters[r_child],
				heap->waiters[sm_or_hi]) == SWAP)
			sm_or_hi = r_child;
		if (sm_or_hi == parent)
			break ;
		swap_waiters(&heap->waiters[parent], &heap->waiters[sm_or_hi]);
		parent = sm_or_hi;
	}
}
