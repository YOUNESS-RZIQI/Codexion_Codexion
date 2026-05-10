/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:39 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:40 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_insert(t_simulation *sim, t_heap *heap, t_heap_node node)
{
	int	i;

	if (heap->size >= MAX_WAITERS)
		return (1);
	i = heap->size;
	heap->waiters[i] = node;
	heap->size++;
	heapify_up(sim, heap);
	return (0);
}


void	heap_extract_min(t_heap *heap)
{
	if (heap->size <= 0)
		return ;
	heap->waiters[0] = heap->waiters[heap->size - 1];
	heap->size--;
	heapify_down(heap);
}

