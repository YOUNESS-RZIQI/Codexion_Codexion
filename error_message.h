#ifndef ERROR_MESSAGE_H
# define ERROR_MESSAGE_H

# include "codexion.h"

short	null_error_message(void)
{
	write(2, "! ! ! Error (type: NULL return) ! ! !\n", 38);
	return (1);
}

short	input_error_message(void)
{
	write(2, "! ! ! Error in Input ! ! !\n", 27);
	return (1);
}

short	initialize_simulation_error_message(void)
{
	write(2, "! ! ! Error in Initialization ! ! !\n", 36);
	return (1);
}

short	mutex_error_message(void)
{
	write(2, "! ! ! Error in Mutex Initialization ! ! !\n", 42);
	return (1);
}

short	thread_creation_fail_error_message(void)
{
	write(2, "! ! ! Error in thread creation ! ! !\n", 37);
	return (1);
}

#endif
