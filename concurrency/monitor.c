/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:25:20 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/05 17:25:21 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_simulation *sim, long long now)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (now >= sim->coders[i].deadline)
			return (sim->coders[i].id);
		i++;
	}
	return (-1);
}

static void	handle_burnout(t_simulation *sim, int burned_id)
{
	log_burnout(sim, burned_id);
	pthread_cond_broadcast(&sim->state_cond);
}

void	*monitor_thread_fn(void *arg)
{
	t_simulation	*sim;
	int				burned_id;

	sim = (t_simulation *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->state_mutex);
		if (!sim->simulation_running)
			break ;
		burned_id = check_burnout(sim, get_time_ms());
		if (burned_id != -1)
		{
			handle_burnout(sim, burned_id);
			pthread_mutex_unlock(&sim->state_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&sim->state_mutex);
		usleep(1000);
	}
	pthread_mutex_unlock(&sim->state_mutex);
	return (NULL);
}
