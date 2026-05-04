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

int	compare_nodes(t_simulation *sim, t_heap_node a, t_heap_node b)
{
	if (a.priority != b.priority)
		return (a.priority - b.priority);
	return (sim->coders[a.coder_number - 1].compile_count - sim->coders[b.coder_number - 1].compile_count);
}

void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	heapify_up(t_simulation *sim, t_heap *heap, int i)
{
	if (i == 1)
		if (compare_nodes(sim, heap->nodes[1], heap->nodes[0]) < 0)
			swap_nodes(&heap->nodes[0], &heap->nodes[1]);
}

void	heapify_down(t_heap *heap)
{
	if (heap->size == 2)
		swap_nodes(&heap->nodes[0], &heap->nodes[1]);
}

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}
