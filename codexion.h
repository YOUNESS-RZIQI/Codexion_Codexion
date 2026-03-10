#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

/*
** t_dongle — one shared USB dongle between two coders
**
**   mutex      : only one coder can touch this dongle at a time
**   cond       : coders WAIT here when dongle is busy/cooling
**   in_use     : 1 = someone holds it, 0 = free
**   cooldown_until : dongle cannot be taken before this timestamp
**   queue / qsize  : who is waiting and in what order (FIFO/EDF)
*/
typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				in_use;
	long			cooldown_until;
	int				queue[256];   /* coder ids waiting (max 256) */
	long			keys[256];    /* scheduling key per waiter   */
	int				qsize;
}	t_dongle;

/*
** t_coder — one coder = one thread
**
**   id               : 1-based number
**   last_compile     : timestamp when last compile STARTED
**                      (monitor uses this to detect burnout)
**   compiles         : how many compiles done so far
**   left / right     : index of the two dongles this coder uses
*/
typedef struct s_coder
{
	int		id;
	long	last_compile;
	int		compiles;
	int		left;
	int		right;
}	t_coder;

/*
** t_sim — the whole simulation state (no globals allowed)
**
**   Passed as pointer to every thread so they share one context.
*/
typedef struct s_sim
{
	int				n;           /* number of coders              */
	long			t_burnout;   /* ms until burnout              */
	long			t_compile;   /* ms to compile                 */
	long			t_debug;     /* ms to debug                   */
	long			t_refactor;  /* ms to refactor                */
	int				required;    /* compiles needed to stop       */
	long			cooldown;    /* ms dongle rests after release */
	int				use_edf;     /* 1=EDF, 0=FIFO                 */
	t_coder			*coders;
	t_dongle		*dongles;
	long			start;       /* simulation start timestamp    */
	int				stop;        /* 1 = simulation over           */
	pthread_mutex_t	log_lock;    /* keeps log lines clean         */
	pthread_mutex_t	stop_lock;   /* protects stop flag            */
}	t_sim;

/* utils.c */
long	now_ms(void);
void	ft_sleep(long ms);
void	log_msg(t_sim *s, int id, char *msg);
int		is_stopped(t_sim *s);

/* dongle.c */
void	take_dongle(t_sim *s, t_coder *c, int idx);
void	drop_dongle(t_sim *s, int idx);

/* coder.c */
void	*coder(void *arg);

/* monitor.c */
void	*monitor(void *arg);

#endif
