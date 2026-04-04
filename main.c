#include <stdio.h>
#include "codexion.h"
#include "sim.h"

int main(int argc, char **argv)
{
    t_args args;
    t_simulation sim;

    args = validate_args(argc, argv);
    if (is_empty_args(args))
    {
        printf("error\n");
        return (1);
    }
    if (!init_simulation(&sim, &args))
    {
        printf("error\n");
        return (1);
    }
    run_simulation(&sim);
    cleanup_simulation(&sim);
    return (0);
}