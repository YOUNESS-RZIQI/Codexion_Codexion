

// it does return a full struct of args if sucess, else empty struct.

#include "codexion.h"

short is_empty_args(t_args args)
{
    if (args.number_of_coders == 0 && args.time_to_burnout == 0 
        && args.time_to_compile == 0 && args.time_to_debug == 0 
        && args.time_to_refactor == 0 && args.number_of_compiles_required == 0 
        && args.dongle_cooldown == 0 && strcmp(args.scheduler, "") == 0)
        return (1);
    return (0);
}

t_args empty_args()
{
    t_args args;

    args.number_of_coders = 0;
    args.time_to_burnout = 0;
    args.time_to_compile = 0;
    args.time_to_debug = 0;
    args.time_to_refactor = 0;
    args.number_of_compiles_required = 0;
    args.dongle_cooldown = 0;
    args.scheduler = "";
    return (args);
}

short check_args(char **argv)
{
    int i = 1;
    while (i < 8)
    {
        if (!is_full_digit(argv[i]))
            return (0);
        i++;
    }

    if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
        return (0);

    return (1);
}

t_args args_converted(char **argv)
{
    t_args args;

    args = empty_args();
    
    if (check_args(argv))
    {
        args.number_of_coders = ft_atoi(argv[1]);
        args.time_to_burnout = ft_atoi(argv[2]);
        args.time_to_compile = ft_atoi(argv[3]);
        args.time_to_debug = ft_atoi(argv[4]);
        args.time_to_refactor = ft_atoi(argv[5]);
        args.number_of_compiles_required = ft_atoi(argv[6]);
        args.dongle_cooldown = ft_atoi(argv[7]);
        args.scheduler = argv[8];
    }
    if (args.number_of_coders <= 0 || args.time_to_burnout <= 0  || args.time_to_compile <= 0 
        || args.time_to_debug <= 0 || args.time_to_refactor <= 0 || args.number_of_compiles_required <= 0
        || args.dongle_cooldown <= 0 )
    {
        args = empty_args();
    }
    return (args);
}

t_args convert_args(int argc, char **argv)
{
    t_args args;

    args = empty_args();

    if (argc == 9)
        args = args_converted(argv);

    return (args);
}
