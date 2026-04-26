#ifndef HEAP_UTILS_H
# define HEAP_UTILS_H

# include "codexion.h"

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
	int	parent;

	while (i != 0)
	{
		parent = (i - 1) / 2;
		if (compare_nodes(heap->nodes[i], heap->nodes[parent], type) < 0)
		{
			swap_nodes(&heap->nodes[i], &heap->nodes[parent]);
			i = parent;
		}
		else
			break ;
	}
}

void	heapify_down(t_heap *heap, int i, t_scheduler type)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size && compare_nodes(heap->nodes[left],
				heap->nodes[smallest], type) < 0)
			smallest = left;
		if (right < heap->size && compare_nodes(heap->nodes[right],
				heap->nodes[smallest], type) < 0)
			smallest = right;
		if (smallest != i)
		{
			swap_nodes(&heap->nodes[i], &heap->nodes[smallest]);
			i = smallest;
		}
		else
			break ;
	}
}

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}

#endif
