/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:22:46 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/05 17:22:53 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}

long long	get_sim_time_ms(t_simulation *sim)
{
	return (get_time_ms() - sim->start_time);
}

void	precise_sleep(long long duration_ms, t_simulation *sim)
{
	long long	start;

	start = get_time_ms();
	while (get_time_ms() - start < duration_ms)
	{
		pthread_mutex_lock(&sim->state_mutex);
		if (!sim->simulation_running)
		{
			pthread_mutex_unlock(&sim->state_mutex);
			break ;
		}
		pthread_mutex_unlock(&sim->state_mutex);
		usleep(100);
	}
}

void	log_state(t_simulation *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->state_mutex);
	if (sim->simulation_running)
	{
		printf("%lld %d %s\n", get_sim_time_ms(sim), coder_id, msg);
	}
	pthread_mutex_unlock(&sim->state_mutex);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	log_burnout(t_simulation *sim, int coder_id)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (sim->simulation_running)
	{
		printf("%lld %d burned out\n", get_sim_time_ms(sim), coder_id);
		sim->simulation_running = 0;
	}
	pthread_mutex_unlock(&sim->log_mutex);
}
