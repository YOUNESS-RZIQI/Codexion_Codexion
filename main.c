
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

    printf("number_of_coders: %lld\n", args.number_of_coders);
    printf("time_to_burnout: %lld\n", args.time_to_burnout);
    printf("time_to_compile: %lld\n", args.time_to_compile);
    printf("time_to_debug: %lld\n", args.time_to_debug);
    printf("time_to_refactor: %lld\n", args.time_to_refactor);
    printf("number_of_compiles_required: %lld\n", args.number_of_compiles_required);
    printf("dongle_cooldown: %lld\n", args.dongle_cooldown);
    printf("scheduler: %s\n", args.scheduler);


    return 0;
}