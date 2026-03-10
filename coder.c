#include "codexion.h"

/* small wrapper so the thread gets both its data and the sim */
typedef struct s_pair { t_coder *c; t_sim *s; } t_pair;

/*
** VISUAL — coder lifecycle:
**
**   take lower-index dongle first  ← prevents deadlock
**   take higher-index dongle second
**          ↓ COMPILING (hold both)
**   drop both dongles (cooldown starts)
**          ↓ DEBUGGING
**          ↓ REFACTORING
**          ↓ (loop back)
*/
void	*coder(void *arg)
{
	t_pair	*p;
	t_sim	*s;
	t_coder	*c;
	int		first;
	int		second;

	p = (t_pair *)arg;
	c = p->c;
	s = p->s;

	/* always take lower index first → breaks circular wait (no deadlock) */
	if (c->left < c->right) { first = c->left;  second = c->right; }
	else                    { first = c->right; second = c->left;  }

	/* 1 coder = 1 dongle = can never hold two = burns out immediately */
	if (s->n == 1)
		return (NULL);

	c->last_compile = s->start;

	while (!is_stopped(s))
	{
		take_dongle(s, c, first);
		if (is_stopped(s)) break ;
		take_dongle(s, c, second);
		if (is_stopped(s)) { drop_dongle(s, first); break ; }

		c->last_compile = now_ms();
		log_msg(s, c->id, "is compiling");
		ft_sleep(s->t_compile);
		c->compiles++;

		drop_dongle(s, first);
		drop_dongle(s, second);
		if (is_stopped(s)) break ;

		log_msg(s, c->id, "is debugging");
		ft_sleep(s->t_debug);
		if (is_stopped(s)) break ;

		log_msg(s, c->id, "is refactoring");
		ft_sleep(s->t_refactor);
	}
	return (NULL);
}
