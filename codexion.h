#ifndef CODEXION_H
#define CODEXION_H

typedef struct s_args
{
    unsigned long long number_of_coders;
    unsigned long long time_to_burnout;
    unsigned long long time_to_compile;
    unsigned long long time_to_debug;
    unsigned long long time_to_refactor;
    unsigned long long number_of_compiles_required;
    unsigned long long dongle_cooldown;
    char *scheduler;

} t_args;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short   is_full_digit(char *s);
int     ft_atoi(const char *s);
t_args  validate_args(int argc, char **argv);
short is_empty_args(t_args args);

#endif