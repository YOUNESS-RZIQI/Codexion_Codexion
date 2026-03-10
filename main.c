#include "codexion.h"

/* small wrapper so coder threads get both their data and the sim */
typedef struct s_pair { t_coder *c; t_sim *s; } t_pair;

/* returns 1 if string is a valid integer >= min */
static int	valid_int(char *str, int min)
{
	int	i;
	int	v;

	i = 0;
	if (str[i] == '+') i++;
	if (!str[i]) return (0);
	while (str[i])
	{ if (str[i] < '0' || str[i] > '9') return (0); i++; }
	v = atoi(str);
	return (v >= min);
}

static int	parse(int ac, char **av, t_sim *s)
{
	if (ac != 9) return (fprintf(stderr,
		"Usage: %s n_coders t_burnout t_compile t_debug t_refactor "
		"n_required cooldown scheduler\n", av[0]), 0);
	if (!valid_int(av[1], 1)) return (fprintf(stderr, "bad n_coders\n"), 0);
	if (!valid_int(av[2], 0)) return (fprintf(stderr, "bad t_burnout\n"), 0);
	if (!valid_int(av[3], 0)) return (fprintf(stderr, "bad t_compile\n"), 0);
	if (!valid_int(av[4], 0)) return (fprintf(stderr, "bad t_debug\n"), 0);
	if (!valid_int(av[5], 0)) return (fprintf(stderr, "bad t_refactor\n"), 0);
	if (!valid_int(av[6], 1)) return (fprintf(stderr, "bad n_required\n"), 0);
	if (!valid_int(av[7], 0)) return (fprintf(stderr, "bad cooldown\n"), 0);
	if (strcmp(av[8], "fifo") != 0 && strcmp(av[8], "edf") != 0)
		return (fprintf(stderr, "scheduler must be fifo or edf\n"), 0);
	s->n         = atoi(av[1]);
	s->t_burnout = atoi(av[2]);
	s->t_compile = atoi(av[3]);
	s->t_debug   = atoi(av[4]);
	s->t_refactor = atoi(av[5]);
	s->required  = atoi(av[6]);
	s->cooldown  = atoi(av[7]);
	s->use_edf   = (strcmp(av[8], "edf") == 0);
	return (1);
}

int	main(int ac, char **av)
{
	t_sim		s;
	t_pair		*pairs;
	pthread_t	*tids;
	int			i;

	memset(&s, 0, sizeof(s));
	if (!parse(ac, av, &s))
		return (1);

	/* allocate coders and dongles */
	s.coders  = malloc(sizeof(t_coder)  * s.n);
	s.dongles = malloc(sizeof(t_dongle) * s.n);
	pairs     = malloc(sizeof(t_pair)   * s.n);
	tids      = malloc(sizeof(pthread_t) * (s.n + 1)); /* +1 for monitor */
	if (!s.coders || !s.dongles || !pairs || !tids)
		return (fprintf(stderr, "malloc failed\n"), 1);

	memset(s.coders,  0, sizeof(t_coder)  * s.n);
	memset(s.dongles, 0, sizeof(t_dongle) * s.n);

	pthread_mutex_init(&s.log_lock,  NULL);
	pthread_mutex_init(&s.stop_lock, NULL);

	/* init each dongle */
	i = 0;
	while (i < s.n)
	{
		pthread_mutex_init(&s.dongles[i].mutex, NULL);
		pthread_cond_init(&s.dongles[i].cond,   NULL);
		i++;
	}

	/* init each coder — circular seating like philosophers */
	i = 0;
	while (i < s.n)
	{
		s.coders[i].id    = i + 1;
		s.coders[i].left  = i;
		s.coders[i].right = (i + 1) % s.n;
		pairs[i].c = &s.coders[i];
		pairs[i].s = &s;
		i++;
	}

	s.start = now_ms();

	/* launch coder threads */
	i = 0;
	while (i < s.n)
	{
		pthread_create(&tids[i], NULL, coder, &pairs[i]);
		i++;
	}
	/* launch monitor thread */
	pthread_create(&tids[s.n], NULL, monitor, &s);

	/* wait for everyone */
	i = 0;
	while (i <= s.n)
	{
		pthread_join(tids[i], NULL);
		i++;
	}

	/* cleanup */
	i = 0;
	while (i < s.n)
	{
		pthread_mutex_destroy(&s.dongles[i].mutex);
		pthread_cond_destroy(&s.dongles[i].cond);
		i++;
	}
	pthread_mutex_destroy(&s.log_lock);
	pthread_mutex_destroy(&s.stop_lock);
	free(s.coders);
	free(s.dongles);
	free(pairs);
	free(tids);
	return (0);
}
