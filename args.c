/*
** args.c
** ============================================================
** WHAT  : Parses and validates the 8 mandatory CLI arguments.
** WHY   : The subject says "Reject invalid inputs such as
**         negative numbers, non-integers, or a scheduler other
**         than fifo or edf."  All checks live here so main()
**         stays clean.
** REMOVE: The program would accept garbage input and behave
**         unpredictably (e.g. negative sleep times → usleep
**         wraps around on some systems).
** ============================================================
*/

#include "codexion.h"

/*
** is_positive_int
** ───────────────
** WHAT : Returns 1 if the string s represents a positive
**        (> 0) integer with no extra characters.
** WHY  : atoi() silently accepts "42abc" → we need a strict
**        check so "5x" is rejected as required.
** REMOVE: Invalid strings like "-3" or "0" would pass.
*/
static int	is_positive_int(const char *s)
{
	int	i;

	i = 0;
	/* Allow an optional leading '+' sign */
	if (s[i] == '+')
		i++;
	/* Must have at least one digit */
	if (!s[i])
		return (0);
	/* Every remaining character must be a digit */
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	/* atoi result must be > 0 (rejects "0" or "00") */
	return (atoi(s) > 0);
}

/*
** is_non_negative_int
** ───────────────────
** WHAT : Same as above but allows 0 (for times and cooldown).
** WHY  : time_to_compile = 0 is theoretically valid (instant
**        compile); the subject only rejects negatives.
*/
static int	is_non_negative_int(const char *s)
{
	int	i;

	i = 0;
	if (s[i] == '+')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (atoi(s) >= 0);
}

/*
** parse_args
** ──────────
** WHAT : Fills an t_args struct from argv[1..8].
** WHY  : Called once from main; keeps all validation in one
**        place rather than scattered around the code.
** RETURN: 1 on success, 0 on any error (message to stderr).
**
** VISUAL — argument positions:
**
**  argv[1] number_of_coders          ← must be > 0
**  argv[2] time_to_burnout           ← must be >= 0
**  argv[3] time_to_compile           ← must be >= 0
**  argv[4] time_to_debug             ← must be >= 0
**  argv[5] time_to_refactor          ← must be >= 0
**  argv[6] number_of_compiles_req    ← must be > 0
**  argv[7] dongle_cooldown           ← must be >= 0
**  argv[8] scheduler                 ← "fifo" or "edf"
*/
int	parse_args(int argc, char **argv, t_args *args)
{
	/* Exactly 9 tokens: program name + 8 arguments */
	if (argc != 9)
	{
		fprintf(stderr,
			"Usage: %s number_of_coders time_to_burnout time_to_compile "
			"time_to_debug time_to_refactor number_of_compiles_required "
			"dongle_cooldown scheduler\n", argv[0]);
		return (0);
	}

	/* Validate each numeric argument */
	if (!is_positive_int(argv[1]))
		return (fprintf(stderr, "Error: number_of_coders must be > 0\n"), 0);
	if (!is_non_negative_int(argv[2]))
		return (fprintf(stderr, "Error: time_to_burnout must be >= 0\n"), 0);
	if (!is_non_negative_int(argv[3]))
		return (fprintf(stderr, "Error: time_to_compile must be >= 0\n"), 0);
	if (!is_non_negative_int(argv[4]))
		return (fprintf(stderr, "Error: time_to_debug must be >= 0\n"), 0);
	if (!is_non_negative_int(argv[5]))
		return (fprintf(stderr, "Error: time_to_refactor must be >= 0\n"), 0);
	if (!is_positive_int(argv[6]))
		return (fprintf(stderr,
				"Error: number_of_compiles_required must be > 0\n"), 0);
	if (!is_non_negative_int(argv[7]))
		return (fprintf(stderr, "Error: dongle_cooldown must be >= 0\n"), 0);

	/* Validate scheduler string — must be exactly "fifo" or "edf" */
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (fprintf(stderr,
				"Error: scheduler must be 'fifo' or 'edf'\n"), 0);

	/* All checks passed — fill the struct */
	args->number_of_coders          = atoi(argv[1]);
	args->time_to_burnout           = (long)atoi(argv[2]);
	args->time_to_compile           = (long)atoi(argv[3]);
	args->time_to_debug             = (long)atoi(argv[4]);
	args->time_to_refactor          = (long)atoi(argv[5]);
	args->number_of_compiles_required = atoi(argv[6]);
	args->dongle_cooldown           = (long)atoi(argv[7]);
	args->scheduler = (strcmp(argv[8], "fifo") == 0) ? FIFO : EDF;

	return (1); /* success */
}
