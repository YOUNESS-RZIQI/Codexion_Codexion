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

int	compare_nodes(t_heap_node a, t_heap_node b, t_scheduler type)
{
	if (type == FIFO)
		return (a.priority - b.priority);
	if (a.priority != b.priority)
		return (a.priority - b.priority);
	return (a.coder_number - b.coder_number);
}

void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	heapify_up(t_heap *heap, int i, t_scheduler type)
{
	if (i == 2)
		if (compare_nodes(heap->nodes[1], heap->nodes[0], type) < 0)
			swap_nodes(&heap->nodes[0], &heap->nodes[1]);
}

void	heapify_down(t_heap *heap)
{
	if (heap->size == 2)
		swap_nodes(&(heap->nodes[0]), &(heap->nodes[1]));
}

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}
