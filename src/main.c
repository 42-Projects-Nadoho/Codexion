/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:22:38 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/07 20:56:48 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	join_coders(t_simulation *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

static int	start_coder_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_thread_fn, &sim->coders[i]) != 0)
		{
			pthread_mutex_lock(&sim->state_mutex);
			sim->simulation_running = 0;
			pthread_cond_broadcast(&sim->state_cond);
			pthread_mutex_unlock(&sim->state_mutex);
			join_coders(sim, i);
			cleanup_simulation(sim);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	start_monitor_thread(t_simulation *sim, pthread_t *monitor)
{
	if (pthread_create(monitor, NULL, monitor_thread_fn, sim) != 0)
	{
		pthread_mutex_lock(&sim->state_mutex);
		sim->simulation_running = 0;
		pthread_cond_broadcast(&sim->state_cond);
		pthread_mutex_unlock(&sim->state_mutex);
		join_coders(sim, sim->number_of_coders);
		cleanup_simulation(sim);
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		monitor_thread;

	memset(&sim, 0, sizeof(t_simulation));
	if (!parse_arguments(&sim, argc, argv))
		return (1);
	if (sim.number_of_compiles_required == 0)
		return (0);
	if (!init_simulation(&sim))
	{
		fprintf(stderr, "Error: Simulation echouee.\n");
		return (1);
	}
	if (!start_coder_threads(&sim))
		return (1);
	if (!start_monitor_thread(&sim, &monitor_thread))
		return (1);
	join_coders(&sim, sim.number_of_coders);
	pthread_join(monitor_thread, NULL);
	cleanup_simulation(&sim);
	return (0);
}
