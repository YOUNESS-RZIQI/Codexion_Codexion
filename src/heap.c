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

int	heap_insert(t_simulation *sim, t_heap *heap, t_heap_node node, t_scheduler type)
{
	int	i;

	if (heap->size >= 2)
		return (1);
	i = heap->size;
	heap->nodes[i] = node;
	heap->size++;
	if (type == EDF)
		heapify_up(sim, heap, i);
	return (0);
}

t_heap_node	heap_peek(t_heap *heap)
{
	return (heap->nodes[0]);
}

void	heap_extract_min(t_heap *heap)
{
	if (heap->size <= 0)
		return ;
	heap->nodes[0] = heap->nodes[heap->size - 1];
	heap->size--;
	heapify_down(heap);
}

void	heap_remove(t_heap *heap, int coder_num)
{
	if (heap->size == 0)
		return ;
	if (heap->nodes[0].coder_number == coder_num)
		heap_extract_min(heap);
	else if (heap->size == 2 && heap->nodes[1].coder_number == coder_num)
		heap->size = 1;
}
