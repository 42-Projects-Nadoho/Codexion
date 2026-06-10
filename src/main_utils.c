/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadoho <nadoho@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 17:24:00 by nadoho            #+#    #+#             */
/*   Updated: 2026/06/10 11:43:41 by nadoho           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_positive_int(const char *str)
{
	int	i;

	if (!str || *str == '\0')
		return (0);
	i = 0;
	if (str[i] == '+')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	validate_args(int argc, char **argv)
{
	int	i;

	if (argc != 9)
	{
		fprintf(stderr, "Error: Invalid number of arguments.\n");
		return (0);
	}
	i = 1;
	while (i <= 7)
	{
		if (!is_positive_int(argv[i]))
		{
			fprintf(stderr, "Error: Argument %d must be non-negative.\n",
				i);
			return (0);
		}
		i++;
	}
	return (1);
}

int	parse_arguments(t_simulation *sim, int argc, char **argv)
{
	if (!validate_args(argc, argv))
		return (0);
	sim->number_of_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->number_of_compiles_required = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	sim->scheduler = argv[8];
	if (sim->number_of_coders < 1)
	{
		fprintf(stderr, "ERREUR: Il doit avoir au moins 1 codeur 1.\n");
		return (0);
	}
	if (strcmp(sim->scheduler, "fifo") != 0
		&& strcmp(sim->scheduler, "edf") != 0)
	{
		fprintf(stderr, "ERREUR: Le gestionnaire doit etre 'fifo' ou 'edf'.\n");
		return (0);
	}
	return (1);
}

void	cleanup_simulation(t_simulation *sim)
{
	int	i;

	if (sim->coders)
	{
		free(sim->coders);
		sim->coders = NULL;
	}
	if (sim->dongles)
	{
		i = 0;
		while (i < sim->number_of_coders)
		{
			heap_destroy(&sim->dongles[i].queue);
			i++;
		}
		free(sim->dongles);
		sim->dongles = NULL;
	}
	pthread_mutex_destroy(&sim->state_mutex);
	pthread_cond_destroy(&sim->state_cond);
	pthread_mutex_destroy(&sim->log_mutex);
}
