/*
** time_utils.c
** ============================================================
** WHAT  : Two small time helpers used everywhere.
** WHY   : Centralising time functions avoids copy-paste and
**         makes it trivial to swap the clock source later.
** ============================================================
*/

#include "codexion.h"

/*
** now_ms
** ──────
** WHAT : Returns the current wall-clock time in milliseconds.
** WHY  : The subject says "use gettimeofday() — real-time
**        measurements are acceptable and recommended for
**        simplicity."  We subtract nothing here; callers
**        subtract sim->start_time for relative timestamps.
** REMOVE: Every timestamp and deadline calculation breaks.
**
** VISUAL — how gettimeofday maps to milliseconds:
**
**  struct timeval tv;
**  tv.tv_sec   = seconds since epoch   (e.g. 1_700_000_000)
**  tv.tv_usec  = microseconds part     (0 .. 999_999)
**
**  ms = tv.tv_sec * 1000  +  tv.tv_usec / 1000
*/
long	now_ms(void)
{
	struct timeval	tv;

	/* gettimeofday fills tv with current time */
	gettimeofday(&tv, NULL);
	/* Convert seconds → ms, then add microseconds → ms */
	return ((long)tv.tv_sec * 1000L + (long)tv.tv_usec / 1000L);
}

/*
** sleep_ms
** ────────
** WHAT : Sleeps for exactly `ms` milliseconds using usleep().
** WHY  : usleep() takes microseconds; wrapping it keeps
**        every call-site readable: sleep_ms(200) not
**        usleep(200000).
** REMOVE: Every sleep call would need inline multiplication,
**         risking off-by-1000 bugs.
** NOTE  : We split into multiple 500 µs slices so the coder
**         thread can be interrupted quickly if sim->stop is
**         checked by the caller after this returns.  For very
**         short sleeps we just call usleep once.
*/
void	sleep_ms(long ms)
{
	/* usleep max is 999999 µs; for longer waits we loop */
	long	remaining_us;

	remaining_us = ms * 1000L; /* convert ms → µs */
	while (remaining_us > 0)
	{
		/* sleep in chunks of at most 500 µs for responsiveness */
		if (remaining_us > 500)
			usleep(500);
		else
			usleep((useconds_t)remaining_us);
		remaining_us -= 500;
	}
}
