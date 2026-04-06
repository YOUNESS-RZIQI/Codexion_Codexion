// if args invalide ?  or One Coder (one dongel) ?


#include "codexion.h"

static short validate_data(t_args data)
{
    if (is_empty_args(data) || data.number_of_coders < 2)
    {
        printf("\n ! ! ! Error in Args ! ! !\n\n");
        return (0);
    }
    return (1);
}

int main(int argc, char **argv)
{
    t_args targs;

    targs = convert_args(argc, argv);

    if(!validate_data(targs))
        return 1;
    printf("\nOK\n");
    return 0;
}
