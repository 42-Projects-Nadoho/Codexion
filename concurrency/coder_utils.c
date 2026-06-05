/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:    +:            */
/*   By: luzolo <luzolo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 08:00:00 by luzolo            #+#    #+#             */
/*   Updated: 2026/06/01 08:00:00 by luzolo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	request_dongles(t_coder *coder, t_simulation *sim)
{
	t_request	req;

	coder->state = CODER_WAITING;
	req.coder_id = coder->id;
	if (strcmp(sim->scheduler, "fifo") == 0)
		req.priority_val = get_time_ms();
	else
		req.priority_val = coder->deadline;
	req.tie_breaker = coder->id;
	heap_push(&sim->dongles[coder->left_dongle_idx].queue, req);
	heap_push(&sim->dongles[coder->right_dongle_idx].queue, req);
}

int	check_all_done(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->coders[i].compile_count < sim->number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

long long	get_wait_until(t_coder *coder, t_simulation *sim,
		long long now)
{
	long long	wait_until;
	t_dongle	*left;
	t_dongle	*right;

	wait_until = coder->deadline;
	left = &sim->dongles[coder->left_dongle_idx];
	right = &sim->dongles[coder->right_dongle_idx];
	if (coder->left_dongle_idx != coder->right_dongle_idx)
	{
		if (left->cooldown_until > now && left->cooldown_until < wait_until)
			wait_until = left->cooldown_until;
		if (right->cooldown_until > now && right->cooldown_until < wait_until)
			wait_until = right->cooldown_until;
	}
	return (wait_until);
}

void	init_coder_deadline(t_coder *coder, t_simulation *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	coder->last_compile_start = sim->start_time;
	coder->deadline = coder->last_compile_start + sim->time_to_burnout;
	pthread_mutex_unlock(&sim->state_mutex);
}

void	cleanup_coder_requests(t_coder *coder, t_simulation *sim)
{
	heap_remove(&sim->dongles[coder->left_dongle_idx].queue, coder->id);
	heap_remove(&sim->dongles[coder->right_dongle_idx].queue, coder->id);
}
