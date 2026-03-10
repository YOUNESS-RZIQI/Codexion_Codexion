/*
** queue.c
** ============================================================
** WHAT  : Min-heap priority queue for dongle arbitration.
** WHY   : The subject says "You must implement a priority
**         queue (heap) for FIFO/EDF scheduling (C89 has no
**         standard library for this)."
**         A min-heap gives O(log n) push and O(log n) pop,
**         which is efficient even for many coders.
** ============================================================
**
** VISUAL — Min-Heap layout in the array:
**
**   Index:    0     1     2     3     4
**   Value:  [10]  [20]  [15]  [25]  [30]
**
**   Tree view:
**          10  (root = minimum)
**         /  \
**        20   15
**       / \
**      25  30
**
**   Parent of index i  → (i - 1) / 2
**   Left child of i    → 2*i + 1
**   Right child of i   → 2*i + 2
**
**   For FIFO: key = arrival_time  (smallest = earliest)
**   For EDF:  key = deadline      (smallest = most urgent)
**
** VISUAL — Push (sift-up):
**   Insert at end → swap upward while key < parent.key
**
** VISUAL — Pop (sift-down):
**   Replace root with last element → swap down to restore heap.
*/

#include "codexion.h"

/* ── Internal helpers ────────────────────────────────────────*/

/*
** swap_req: swaps two queue entries (used during heap ops).
** WHY: heap restructuring needs to move entries around.
*/
static void	swap_req(t_req *a, t_req *b)
{
	t_req	tmp;

	tmp = *a;
	*a  = *b;
	*b  = tmp;
}

/*
** sift_up
** ───────
** WHAT : After inserting at index `i`, bubbles it up until
**        the heap property (parent.key ≤ child.key) holds.
** WHY  : Maintains min-heap invariant after a push.
*/
static void	sift_up(t_dongle *d, int i)
{
	int	parent;

	/* While not at root and our key is smaller than parent's key */
	while (i > 0)
	{
		parent = (i - 1) / 2; /* formula for parent index */
		if (d->queue[i].key < d->queue[parent].key)
		{
			swap_req(&d->queue[i], &d->queue[parent]);
			i = parent; /* move up to check again */
		}
		else
			break; /* heap property satisfied */
	}
}

/*
** sift_down
** ─────────
** WHAT : After removing root and placing last element there,
**        pushes it down until heap property is restored.
** WHY  : Maintains min-heap invariant after a pop.
*/
static void	sift_down(t_dongle *d, int i)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left     = 2 * i + 1; /* left child index  */
		right    = 2 * i + 2; /* right child index */
		smallest = i;          /* assume current is smallest */

		/* Check if left child exists and is smaller */
		if (left < d->queue_size
			&& d->queue[left].key < d->queue[smallest].key)
			smallest = left;

		/* Check if right child exists and is smaller */
		if (right < d->queue_size
			&& d->queue[right].key < d->queue[smallest].key)
			smallest = right;

		/* If current is already smallest, heap is valid */
		if (smallest == i)
			break;

		/* Otherwise swap and continue down */
		swap_req(&d->queue[i], &d->queue[smallest]);
		i = smallest;
	}
}

/* ── Public API ──────────────────────────────────────────────*/

/*
** queue_push
** ──────────
** WHAT : Inserts a new request into the dongle's priority queue.
** WHY  : Called when a coder starts waiting for the dongle.
** RETURN: 1 on success, 0 on allocation failure.
*/
int	queue_push(t_dongle *d, t_req req)
{
	t_req	*new_queue;
	int		new_cap;

	/* Grow the backing array when full */
	if (d->queue_size >= d->queue_cap)
	{
		/* Double capacity (or start at 4) */
		new_cap   = (d->queue_cap == 0) ? 4 : d->queue_cap * 2;
		new_queue = (t_req *)malloc(sizeof(t_req) * new_cap);
		if (!new_queue)
			return (0); /* allocation failed */

		/* Copy existing entries then free old array */
		if (d->queue)
		{
			memset(new_queue, 0, sizeof(t_req) * new_cap);
			memcpy(new_queue, d->queue, sizeof(t_req) * d->queue_size);
			free(d->queue);
		}
		d->queue     = new_queue;
		d->queue_cap = new_cap;
	}

	/* Place new entry at end and sift it up */
	d->queue[d->queue_size] = req;
	d->queue_size++;
	sift_up(d, d->queue_size - 1);
	return (1);
}

/*
** queue_pop
** ─────────
** WHAT : Removes and returns the minimum-key entry (root).
** WHY  : Called when the dongle is granted to the best coder.
** NOTE : Caller must ensure queue is non-empty.
*/
t_req	queue_pop(t_dongle *d)
{
	t_req	top;

	top = d->queue[0]; /* save root (minimum) */

	/* Move last element to root position */
	d->queue_size--;
	if (d->queue_size > 0)
	{
		d->queue[0] = d->queue[d->queue_size];
		sift_down(d, 0); /* restore heap property */
	}
	return (top);
}

/*
** queue_peek_id
** ─────────────
** WHAT : Returns the coder_id at the front of the queue
**        without removing it.  Returns -1 if empty.
** WHY  : The dongle uses this to check who should be woken.
*/
int	queue_peek_id(t_dongle *d)
{
	if (d->queue_size == 0)
		return (-1);
	return (d->queue[0].coder_id);
}
