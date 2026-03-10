/*
** log.c
** ============================================================
** WHAT  : Single function for all simulation output.
** WHY   : The subject says "A displayed state message should
**         not be mixed up with another message."  A mutex
**         around every printf ensures atomic output.
** REMOVE: Two threads might write simultaneously, producing
**         garbled lines like "10 1 is 15 2 is compilingdebugging"
** ============================================================
**
** VISUAL — without mutex (BAD):
**
**  Thread 1: printf("10 1 is compiling")
**                    ← context switch mid-print
**  Thread 2:                             printf("11 2 is debugging")
**  Output : "10 1 is 11 2 is debuggingcompiling"  ← garbled!
**
** VISUAL — with log_mutex (GOOD):
**
**  Thread 1: lock → print "10 1 is compiling\n" → unlock
**  Thread 2: (waits) → lock → print "11 2 is debugging\n" → unlock
**  Output : "10 1 is compiling\n11 2 is debugging\n"  ← clean!
*/

#include "codexion.h"

/*
** log_event
** ─────────
** WHAT : Prints "timestamp coder_id msg\n" atomically.
** WHY  : timestamp is relative to sim start so it matches
**        the subject's example output format.
** ARGS :
**   sim      — needed for start_time and log_mutex
**   coder_id — the 1-based coder number (printed as X)
**   msg      — one of the five legal state strings
*/
void	log_event(t_sim *sim, int coder_id, const char *msg)
{
	long	ts; /* relative timestamp in ms */

	/* Compute relative timestamp BEFORE locking so the stamp
	   reflects when the event actually happened, not when the
	   mutex became available. */
	ts = now_ms() - sim->start_time;

	/* Lock: no other thread may write until we are done */
	pthread_mutex_lock(&sim->log_mutex);

	/* Print in exact format required by the subject:
	   "timestamp_in_ms X <message>"                      */
	printf("%ld %d %s\n", ts, coder_id, msg);

	/* Unlock: next thread may now write */
	pthread_mutex_unlock(&sim->log_mutex);
}
