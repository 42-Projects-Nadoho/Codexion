/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:25:14 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/05 17:25:15 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *h, int capacity)
{
	h->data = malloc(capacity * sizeof(t_request));
	if (!h->data)
		return (0);
	h->size = 0;
	h->capacity = capacity;
	return (1);
}

void	heap_destroy(t_heap *h)
{
	if (h->data)
	{
		free(h->data);
		h->data = NULL;
	}
	h->size = 0;
	h->capacity = 0;
}

static int	heap_resize(t_heap *h)
{
	int			new_capacity;
	t_request	*new_data;

	new_capacity = h->capacity * 2;
	new_data = malloc(new_capacity * sizeof(t_request));
	if (!new_data)
		return (0);
	memcpy(new_data, h->data, h->size * sizeof(t_request));
	free(h->data);
	h->data = new_data;
	h->capacity = new_capacity;
	return (1);
}

int	heap_push(t_heap *h, t_request req)
{
	int	i;
	int	parent;

	if (h->size >= h->capacity && !heap_resize(h))
		return (0);
	h->data[h->size] = req;
	i = h->size;
	h->size++;
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
	return (1);
}

int	heap_peek(const t_heap *h, t_request *out_req)
{
	if (h->size <= 0)
		return (0);
	*out_req = h->data[0];
	return (1);
}
