#include "codexion.h"

/* current time in milliseconds */
long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)tv.tv_sec * 1000L + (long)tv.tv_usec / 1000L);
}

/* sleep ms milliseconds in small chunks so stop is checked often */
void	ft_sleep(long ms)
{
	long	end;

	end = now_ms() + ms;
	while (now_ms() < end)
		usleep(500);
}

/* print one log line — mutex keeps lines from mixing */
void	log_msg(t_sim *s, int id, char *msg)
{
	long	ts;

	ts = now_ms() - s->start;
	pthread_mutex_lock(&s->log_lock);
	printf("%ld %d %s\n", ts, id, msg);
	pthread_mutex_unlock(&s->log_lock);
}

/* thread-safe read of the stop flag */
int	is_stopped(t_sim *s)
{
	int	v;

	pthread_mutex_lock(&s->stop_lock);
	v = s->stop;
	pthread_mutex_unlock(&s->stop_lock);
	return (v);
}
