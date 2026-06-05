/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:    +:            */
/*   By: luzolo <luzolo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 08:00:00 by luzolo            #+#    #+#             */
/*   Updated: 2026/06/01 08:00:00 by luzolo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int	is_higher_priority(t_request a, t_request b)
{
	if (a.priority_val < b.priority_val)
		return (1);
	if (a.priority_val > b.priority_val)
		return (0);
	return (a.tie_breaker < b.tie_breaker);
}
