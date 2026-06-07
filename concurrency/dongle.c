/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:24:54 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/07 22:41:01 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	bubble_up(t_heap *h, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (is_higher_priority(h->data[i], h->data[parent]))
		{
			swap_requests(&h->data[i], &h->data[parent]);
			i = parent;
		}
		else
			break ;
	}
}

static void	bubble_down(t_heap *h, int i)
{
	int	left;
	int	right;
	int	best;

	while (2 * i + 1 < h->size)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		best = i;
		if (is_higher_priority(h->data[left], h->data[best]))
			best = left;
		if (right < h->size
			&& is_higher_priority(h->data[right], h->data[best]))
			best = right;
		if (best != i)
		{
			swap_requests(&h->data[i], &h->data[best]);
			i = best;
		}
		else
			break ;
	}
}

void	heap_remove(t_heap *h, int coder_id)
{
	int	idx;
	int	i;

	idx = -1;
	i = 0;
	while (i < h->size)
	{
		if (h->data[i].coder_id == coder_id)
		{
			idx = i;
			break ;
		}
		i++;
	}
	if (idx == -1)
		return ;
	h->size--;
	if (idx == h->size)
		return ;
	h->data[idx] = h->data[h->size];
	bubble_up(h, idx);
	bubble_down(h, idx);
}
