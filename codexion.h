#ifndef CODEXION_H
#define CODEXION_H

typedef struct s_args
{
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    char *scheduler;

} t_args;


#include <stdio.h>

int    ft_atoi(const char *str);
short   is_full_digit(char *s);
t_args  validate_args(int argc, char **argv);
short is_empty_args(t_args args);
int    ft_strcmp(char *s1, char *s2);

#endif