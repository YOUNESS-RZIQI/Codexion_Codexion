#include "codexion.h"

/*
** Polls every 1 ms — guarantees burnout log within 1 ms of
** actual deadline, well inside the required 10 ms window.
*/
void	*monitor(void *arg)
{
	t_sim	*s;
	int		i;
	int		all_done;

	s = (t_sim *)arg;
	while (1)
	{
		usleep(1000); /* 1 ms */

		if (is_stopped(s))
			return (NULL);

		/* check every coder for burnout */
		i = 0;
		while (i < s->n)
		{
			if (now_ms() > s->coders[i].last_compile + s->t_burnout)
			{
				log_msg(s, s->coders[i].id, "burned out");
				pthread_mutex_lock(&s->stop_lock);
				s->stop = 1;
				pthread_mutex_unlock(&s->stop_lock);
				/* wake all waiting threads so they can exit */
				i = 0;
				while (i < s->n)
				{
					pthread_mutex_lock(&s->dongles[i].mutex);
					pthread_cond_broadcast(&s->dongles[i].cond);
					pthread_mutex_unlock(&s->dongles[i].mutex);
					i++;
				}
				return (NULL);
			}
			i++;
		}

		/* check if all coders compiled enough times */
		all_done = 1;
		i = 0;
		while (i < s->n)
		{
			if (s->coders[i].compiles < s->required)
			{ all_done = 0; break ; }
			i++;
		}
		if (all_done)
		{
			pthread_mutex_lock(&s->stop_lock);
			s->stop = 1;
			pthread_mutex_unlock(&s->stop_lock);
			i = 0;
			while (i < s->n)
			{
				pthread_mutex_lock(&s->dongles[i].mutex);
				pthread_cond_broadcast(&s->dongles[i].cond);
				pthread_mutex_unlock(&s->dongles[i].mutex);
				i++;
			}
			return (NULL);
		}
	}
}
