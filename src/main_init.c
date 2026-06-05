/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_init.c                                        :+:      :+:    :+:   */
/*                                                    +:    +:            */
/*   By: luzolo <luzolo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 08:00:00 by luzolo            #+#    #+#             */
/*   Updated: 2026/06/01 08:00:00 by luzolo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_mutexes(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (0);
	if (pthread_cond_init(&sim->state_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		return (0);
	}
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		pthread_cond_destroy(&sim->state_cond);
		return (0);
	}
	return (1);
}

static int	init_dongles(t_simulation *sim)
{
	int	i;
	int	j;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->dongles[i].id = i;
		sim->dongles[i].is_held = 0;
		sim->dongles[i].holding_coder_id = 0;
		sim->dongles[i].cooldown_until = 0;
		if (!heap_init(&sim->dongles[i].queue, 4))
		{
			j = 0;
			while (j < i)
			{
				heap_destroy(&sim->dongles[j].queue);
				j++;
			}
			return (0);
		}
		i++;
	}
	return (1);
}

static void	init_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].state = CODER_REFACTORING;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].deadline = sim->start_time + sim->time_to_burnout;
		sim->coders[i].left_dongle_idx = i;
		sim->coders[i].right_dongle_idx = (i + 1) % sim->number_of_coders;
		i++;
	}
}

int	init_simulation(t_simulation *sim)
{
	sim->start_time = get_time_ms();
	sim->simulation_running = 1;
	if (!init_mutexes(sim))
		return (0);
	sim->coders = malloc(sim->number_of_coders * sizeof(t_coder));
	sim->dongles = malloc(sim->number_of_coders * sizeof(t_dongle));
	if (!sim->coders || !sim->dongles)
	{
		cleanup_simulation(sim);
		return (0);
	}
	if (!init_dongles(sim))
	{
		cleanup_simulation(sim);
		return (0);
	}
	init_coders(sim);
	return (1);
}
