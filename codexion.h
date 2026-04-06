#ifndef CODEXION_H
#define CODEXION_H

typedef struct s_args
{
    long long number_of_coders;
    long long time_to_burnout;
    long long time_to_compile;
    long long time_to_debug;
    long long time_to_refactor;
    long long number_of_compiles_required;
    long long dongle_cooldown;
    char *scheduler;

} t_args;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short   is_full_digit(char *s);
long long     ft_atoi(const char *s);
t_args  convert_args(int argc, char **argv);
short is_empty_args(t_args args);

#endif