#include <unistd.h>

short is_full_digit(char *s)
{
    while (*s)
    {
        if (*s < '0' || *s > '9')
            return (0);
        s++;
    }
    return (1);
}

static short    ft_is_space(char c)
{
        return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'
                || c == '\v');
}

static  int     conversion(const char *s, short sign)
{
        long    _atoi;
        long    befor;

        _atoi = 0;
        while (ft_isdigit((unsigned char)*s))
        {
                befor = _atoi;
                _atoi = _atoi * 10 + (*s - '0');
                if (befor != _atoi / 10)
                {
                        if (sign == -1)
                                return (0);
                        return (-1);
                }
                s++;
        }
        return ((int)_atoi * sign);
}

int     ft_atoi(const char *s)
{
        short   sign;

        if (!s)
                return (0);
        sign = 1;
        while (ft_is_space((unsigned char)*s))
                s++;
        if (*s == '+' || *s == '-')
        {
                if (*s == '-')
                        sign = (sign * -1);
                s++;
        }
        return (conversion(s, sign));
}