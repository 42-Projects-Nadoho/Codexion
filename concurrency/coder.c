/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:24:44 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/10 12:18:10 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_top_priority(t_coder *coder, t_dongle *left, t_dongle *right)
{
	t_request	left_top;
	t_request	right_top;

	if (heap_peek(&left->queue, &left_top) && left_top.coder_id == coder->id
		&& heap_peek(&right->queue, &right_top)
		&& right_top.coder_id == coder->id)
		return (1);
	return (0);
}

int	try_acquire_dongles(t_coder *coder, t_simulation *sim)
{
	long long	now;
	t_dongle	*left;
	t_dongle	*right;

	if (coder->left_dongle_idx == coder->right_dongle_idx)
		return (0);
	now = get_time_ms();
	left = &sim->dongles[coder->left_dongle_idx];
	right = &sim->dongles[coder->right_dongle_idx];
	if (is_top_priority(coder, left, right)
		&& !left->is_held && !right->is_held
		&& now >= left->cooldown_until && now >= right->cooldown_until)
	{
		left->is_held = 1;
		left->holding_coder_id = coder->id;
		right->is_held = 1;
		right->holding_coder_id = coder->id;
		heap_remove(&left->queue, coder->id);
		heap_remove(&right->queue, coder->id);
		return (1);
	}
	return (0);
}

void	wait_for_dongles(t_coder *coder, t_simulation *sim)
{
	long long		wait_until;
	struct timespec	ts;

	while (sim->simulation_running)
	{
		if (try_acquire_dongles(coder, sim))
			break ;
		wait_until = get_wait_until(coder, sim, get_time_ms());
		ts.tv_sec = wait_until / 1000;
		ts.tv_nsec = (wait_until % 1000) * 1000000;
		pthread_cond_timedwait(&sim->state_cond, &sim->state_mutex, &ts);
	}
}

static int	check_running_lock(t_simulation *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	if (!sim->simulation_running)
	{
		pthread_mutex_unlock(&sim->state_mutex);
		return (0);
	}
	return (1);
}

void	*coder_thread_fn(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	init_coder_deadline(coder, sim);
	while (check_running_lock(sim))
	{
		request_dongles(coder, sim);
		wait_for_dongles(coder, sim);
		if (!sim->simulation_running)
		{
			cleanup_coder_requests(coder, sim);
			pthread_mutex_unlock(&sim->state_mutex);
			break ;
		}
		pthread_mutex_unlock(&sim->state_mutex);
		coder_compile_phase(coder, sim);
		coder_debug_refactor_phase(coder, sim);
	}
	return (NULL);
}
