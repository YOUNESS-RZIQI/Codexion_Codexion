/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_message.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrziqi <yrziqi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:03:33 by yrziqi            #+#    #+#             */
/*   Updated: 2026/04/30 13:03:34 by yrziqi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	null_error_message(void)
{
	write(2, "! ! ! Error (type: NULL return) ! ! !\n", 38);
	return (1);
}

bool	input_error_message(void)
{
	write(2, "! ! ! Error in Input ! ! !\n", 27);
	return (1);
}

bool	initialize_simulation_error_message(void)
{
	write(2, "! ! ! Error in Initialization ! ! !\n", 36);
	return (1);
}

bool	mutex_error_message(void)
{
	write(2, "! ! ! Error in Mutex Initialization ! ! !\n", 42);
	return (1);
}

bool	thread_creation_fail_error_message(void)
{
	write(2, "! ! ! Error in thread creation ! ! !\n", 37);
	return (1);
}
