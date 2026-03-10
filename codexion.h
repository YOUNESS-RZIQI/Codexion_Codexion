/*
** codexion.h
** ============================================================
** WHAT  : Master header — defines every type, struct, and
**         function prototype used across the project.
** WHY   : Centralising declarations in one header avoids
**         redefinition errors and keeps every .c file aware
**         of shared structures.
** REMOVE: The project would not compile; every .c file
**         includes this header.
** ============================================================
**
** VISUAL DIAGRAM — Data-structure relationships:
**
**  t_args  ──────────────────────────────────────────────────
**  (parsed CLI arguments, read-only after startup)
**
**  t_dongle  [0 .. N-1]   ←── array of N dongles
**  ┌──────────────────┐
**  │ mutex            │  protects dongle state
**  │ cond             │  used to wake waiting coders
**  │ available        │  1 = free, 0 = in use / cooling
**  │ cooldown_until   │  ms timestamp when cooldown ends
**  │ queue            │  priority queue (FIFO or EDF)
**  │ queue_size       │
**  └──────────────────┘
**
**  t_coder  [0 .. N-1]   ←── one per coder / thread
**  ┌──────────────────┐
**  │ id               │  1-based coder number
**  │ last_compile_start│ ms timestamp, used for EDF deadline
**  │ compile_count    │  how many compiles done so far
**  │ left / right     │  index of the two dongles
**  └──────────────────┘
**
**  t_sim   (single global-free simulation context)
**  ┌──────────────────┐
**  │ coders[]         │  array of t_coder
**  │ dongles[]        │  array of t_dongle
**  │ args             │  parsed CLI arguments
**  │ stop             │  flag: 1 = simulation must end
**  │ log_mutex        │  serialises all stdout writes
**  │ monitor_mutex    │  protects stop flag
**  │ start_time       │  ms timestamp of simulation start
**  └──────────────────┘
*/

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>      /* POSIX threads, mutex, cond vars  */
# include <sys/time.h>     /* gettimeofday()                   */
# include <stdio.h>        /* printf, fprintf                  */
# include <stdlib.h>       /* malloc, free, atoi               */
# include <string.h>       /* strcmp, strlen, memset           */
# include <unistd.h>       /* usleep, write                    */

/* ── Scheduler policy ───────────────────────────────────────*/
typedef enum e_scheduler
{
	FIFO,   /* First-In First-Out — serve by arrival order    */
	EDF     /* Earliest-Deadline-First — serve by urgency     */
}	t_scheduler;

/* ── CLI arguments (parsed once, never written again) ───────*/
typedef struct s_args
{
	int			number_of_coders;       /* N coders & N dongles */
	long		time_to_burnout;        /* ms before a coder burns out  */
	long		time_to_compile;        /* ms held compiling            */
	long		time_to_debug;          /* ms spent debugging           */
	long		time_to_refactor;       /* ms spent refactoring         */
	int			number_of_compiles_required; /* stop threshold          */
	long		dongle_cooldown;        /* ms a dongle rests after use  */
	t_scheduler	scheduler;              /* FIFO or EDF                  */
}	t_args;

/* ── One entry in the dongle's priority queue ───────────────*/
/*
** WHY a queue entry: several coders may request the same dongle
** simultaneously.  We record who requested it and when (or what
** their deadline is) so we can grant it fairly.
*/
typedef struct s_req
{
	int		coder_id;       /* 1-based id of the requesting coder   */
	long	key;            /* FIFO: arrival time  |  EDF: deadline  */
}	t_req;

/* ── Dongle (shared resource, one mutex+cond per dongle) ────*/
/*
** WHY separate mutex per dongle: coders on opposite sides of the
** table should not block each other; fine-grained locking
** reduces unnecessary contention.
*/
typedef struct s_dongle
{
	pthread_mutex_t	mutex;          /* guards all fields below          */
	pthread_cond_t	cond;           /* signals waiting coder threads    */
	int				available;      /* 1 = can be taken, 0 = busy/cool  */
	long			cooldown_until; /* ms timestamp: free after this    */
	t_req			*queue;         /* dynamic array — priority queue   */
	int				queue_size;     /* current number of entries        */
	int				queue_cap;      /* allocated capacity               */
}	t_dongle;

/* ── One coder (one POSIX thread) ───────────────────────────*/
typedef struct s_coder
{
	int				id;                 /* 1-based coder number             */
	long			last_compile_start; /* ms: used for deadline & burnout  */
	int				compile_count;      /* incremented after each compile   */
	int				left;               /* index into dongles[] array       */
	int				right;              /* index into dongles[] array       */
	struct s_sim	*sim;               /* back-pointer to simulation ctx   */
}	t_coder;

/* ── Simulation context (passed everywhere instead of globals)*/
/*
** WHY: global variables are FORBIDDEN by the subject.
** Everything lives here; each thread receives a pointer to sim.
*/
typedef struct s_sim
{
	t_args			args;
	t_coder			*coders;        /* array [0 .. N-1]                 */
	t_dongle		*dongles;       /* array [0 .. N-1]                 */
	int				stop;           /* 1 = simulation over              */
	pthread_mutex_t	log_mutex;      /* serialises all log output        */
	pthread_mutex_t	monitor_mutex;  /* protects stop flag               */
	long			start_time;     /* ms timestamp at sim launch       */
}	t_sim;

/* ── Function prototypes ─────────────────────────────────────*/

/* args.c */
int		parse_args(int argc, char **argv, t_args *args);

/* time_utils.c */
long	now_ms(void);
void	sleep_ms(long ms);

/* log.c */
void	log_event(t_sim *sim, int coder_id, const char *msg);

/* dongle.c */
int		init_dongles(t_sim *sim);
void	destroy_dongles(t_sim *sim);
void	take_dongle(t_sim *sim, t_coder *coder, int dongle_idx);
void	release_dongle(t_sim *sim, int dongle_idx);

/* queue.c */
int		queue_push(t_dongle *d, t_req req);
t_req	queue_pop(t_dongle *d);
int		queue_peek_id(t_dongle *d);

/* coder.c */
void	*coder_routine(void *arg);

/* monitor.c */
void	*monitor_routine(void *arg);

/* sim.c */
int		init_coders(t_sim *sim);
int		run_simulation(t_sim *sim);
void	cleanup_sim(t_sim *sim);

#endif
