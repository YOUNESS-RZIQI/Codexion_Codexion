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

bool	compare_nodes(t_simulation *sim, t_heap_node shiled, t_heap_node parent)
{
	if (sim->args.scheduler_type == FIFO)
		return 0;
	if (shiled.priority < parent.priority)
		return (0);
	if (shiled.priority > parent.priority)
		return (1);
	if (shiled.compile_count < parent.compile_count)
		return (0);
	if (shiled.compile_count > parent.compile_count)
		return (1);
	if (shiled.coder_number < parent.coder_number)
		return (0);
	if (shiled.coder_number > parent.coder_number)
		return (1);
	return (1);
}

void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	heapify_up(t_simulation *sim, t_heap *heap)
{
	if (heap->size > 2)
		return;
}

void	heapify_down(t_heap *heap)
{
	if (heap->size == 2)
		swap_nodes(&heap->nodes[0], &heap->nodes[1]);
}

