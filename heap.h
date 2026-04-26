#ifndef HEAP_H
# define HEAP_H

# include "codexion.h"

int	heap_insert(t_heap *heap, t_heap_node node, t_scheduler type)
{
	int	i;

	if (heap->size >= 2)
		return (1);
	i = heap->size;
	heap->nodes[i] = node;
	heap->size++;
	heapify_up(heap, i, type);
	return (0);
}

t_heap_node	heap_peek(t_heap *heap)
{
	return (heap->nodes[0]);
}

void	heap_extract_min(t_heap *heap, t_scheduler type)
{
	if (heap->size <= 0)
		return ;
	if (heap->size == 1)
	{
		heap->size--;
		return ;
	}
	heap->nodes[0] = heap->nodes[heap->size - 1];
	heap->size--;
	heapify_down(heap, 0, type);
}

#endif
