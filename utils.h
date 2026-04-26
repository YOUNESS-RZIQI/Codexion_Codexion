#ifndef UTILS_H
#define UTILS_H

#include "codexion.h"

short	is_full_digit(char *s)
{
	int	i;

	i = 1;
	while (s[i] == ' ' || s[i] == '\t')
		i++;
	if (s[i] == '-' || s[i] == '+')
		i++;
	while (s[i] >= '0' && s[i] <= '9')
		i++;
	while (s[i] == ' ' || s[i] == '\t')
		i++;
	if (s[i])
		return (0);
	return (1);
}

static short	ft_is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'
		|| c == '\v');
}

static short	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

static long long	conversion(const char *s, short sign)
{
	int	_atoi;
	int	befor;

	_atoi = 0;
	while (ft_isdigit((unsigned char)*s))
	{
		befor = _atoi;
		_atoi = _atoi * 10 + (*s - '0');
		if (befor != _atoi / 10)
			return (0);
		s++;
	}
	return (_atoi * sign);
}

long long	ft_atoi(const char *s)
{
	short	sign;

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

#endif
