/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:    +:            */
/*   By: luzolo <luzolo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 08:00:00 by luzolo            #+#    #+#             */
/*   Updated: 2026/06/01 08:00:00 by luzolo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

typedef enum e_coder_state
{
	CODER_WAITING,
	CODER_COMPILING,
	CODER_DEBUGGING,
	CODER_REFACTORING
}	t_coder_state;

typedef struct s_request
{
	int			coder_id;
	long long	priority_val;
	long long	tie_breaker;
}	t_request;

typedef struct s_heap
{
	t_request	*data;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	int				is_held;
	int				holding_coder_id;
	long long		cooldown_until;
	t_heap			queue;
}	t_dongle;

typedef struct s_simulation	t_simulation;

typedef struct s_coder
{
	int					id;
	t_simulation		*sim;
	pthread_t			thread;
	t_coder_state		state;
	int					compile_count;
	long long			last_compile_start;
	long long			deadline;
	int					left_dongle_idx;
	int					right_dongle_idx;
}	t_coder;

typedef struct s_simulation
{
	int				number_of_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				number_of_compiles_required;
	long long		dongle_cooldown;
	const char		*scheduler;
	long long		start_time;
	int				simulation_running;
	pthread_mutex_t	state_mutex;
	pthread_cond_t	state_cond;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_simulation;

/* Heap utility operations */
void		swap_requests(t_request *a, t_request *b);
int			is_higher_priority(t_request a, t_request b);

/* Heap operations */
int			heap_init(t_heap *h, int capacity);
void		heap_destroy(t_heap *h);
int			heap_push(t_heap *h, t_request req);
int			heap_peek(const t_heap *h, t_request *out_req);
void		heap_remove(t_heap *h, int coder_id);

/* Time and sleep utils */
long long	get_time_ms(void);
long long	get_sim_time_ms(t_simulation *sim);
void		precise_sleep(long long duration_ms, t_simulation *sim);

/* Logger */
void		log_state(t_simulation *sim, int coder_id, const char *msg);
void		log_burnout(t_simulation *sim, int coder_id);

/* Coder utility functions */
void		request_dongles(t_coder *coder, t_simulation *sim);
int			check_all_done(t_simulation *sim);
long long	get_wait_until(t_coder *coder, t_simulation *sim, long long now);
void		init_coder_deadline(t_coder *coder, t_simulation *sim);
void		cleanup_coder_requests(t_coder *coder, t_simulation *sim);

/* Coder phase functions */
void		coder_compile_phase(t_coder *coder, t_simulation *sim);
void		coder_debug_refactor_phase(t_coder *coder, t_simulation *sim);
void		release_dongles(t_coder *coder, t_simulation *sim);
int			try_acquire_dongles(t_coder *coder, t_simulation *sim);
void		wait_for_dongles(t_coder *coder, t_simulation *sim);

/* Thread functions */
void		*coder_thread_fn(void *arg);
void		*monitor_thread_fn(void *arg);

/* Init and Main helpers */
int			parse_arguments(t_simulation *sim, int argc, char **argv);
void		cleanup_simulation(t_simulation *sim);
int			init_simulation(t_simulation *sim);
void		join_coders(t_simulation *sim, int count);

#endif
