/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_phases.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:24:25 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/10 11:58:14 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_dongles(t_coder *coder, t_simulation *sim)
{
	long long	release_time;
	t_dongle	*left;
	t_dongle	*right;

	release_time = get_time_ms();
	left = &sim->dongles[coder->left_dongle_idx];
	right = &sim->dongles[coder->right_dongle_idx];
	left->is_held = 0;
	left->holding_coder_id = 0;
	left->cooldown_until = release_time + sim->dongle_cooldown;
	right->is_held = 0;
	right->holding_coder_id = 0;
	right->cooldown_until = release_time + sim->dongle_cooldown;
	coder->compile_count++;
	coder->state = CODER_DEBUGGING;
	if (check_all_done(sim))
		sim->simulation_running = 0;
	pthread_cond_broadcast(&sim->state_cond);
}

void	coder_compile_phase(t_coder *coder, t_simulation *sim)
{
	long long	compile_start;

	pthread_mutex_lock(&sim->state_mutex);
	coder->state = CODER_COMPILING;
	compile_start = get_time_ms();
	coder->last_compile_start = compile_start;
	coder->deadline = compile_start + sim->time_to_burnout;
	pthread_mutex_unlock(&sim->state_mutex);
	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "is compiling");
	precise_sleep(sim->time_to_compile, sim);
	pthread_mutex_lock(&sim->state_mutex);
	release_dongles(coder, sim);
	pthread_mutex_unlock(&sim->state_mutex);
}

void	coder_debug_refactor_phase(t_coder *coder, t_simulation *sim)
{
	log_state(sim, coder->id, "is debugging");
	precise_sleep(sim->time_to_debug, sim);
	pthread_mutex_lock(&sim->state_mutex);
	if (!sim->simulation_running)
	{
		pthread_mutex_unlock(&sim->state_mutex);
		return ;
	}
	coder->state = CODER_REFACTORING;
	pthread_mutex_unlock(&sim->state_mutex);
	log_state(sim, coder->id, "is refactoring");
	precise_sleep(sim->time_to_refactor, sim);
}
