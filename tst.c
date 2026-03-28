
#include <stdio.h>
#include "codexion.h"

int main(int argc, char **argv)
{
    t_args args;

    args = validate_args(argc, argv);
    if (is_empty_args(args))
        printf("error\n");
    else
        printf("ok\n");

    printf("number_of_coders: %d\n", args.number_of_coders);
    printf("time_to_burnout: %d\n", args.time_to_burnout);
    printf("time_to_compile: %d\n", args.time_to_compile);
    printf("time_to_debug: %d\n", args.time_to_debug);
    printf("time_to_refactor: %d\n", args.time_to_refactor);
    printf("number_of_compiles_required: %d\n", args.number_of_compiles_required);
    printf("dongle_cooldown: %d\n", args.dongle_cooldown);
    printf("scheduler: %s\n", args.scheduler);


    return 0;
}