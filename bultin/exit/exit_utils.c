
#include "minishell.h"

static unsigned long long	get_limit(int sign)
{
	if (sign == 1)
		return ((unsigned long long)LLONG_MAX);
	return ((unsigned long long)LLONG_MAX + 1ULL);
}

static int	process_digits(const char *s, size_t i, unsigned long long limit)
{
	unsigned long long	n;
	unsigned long long	d;

	n = 0;
	while (s[i] && ft_isdigit((unsigned char)s[i]))
	{
		d = (unsigned long long)(s[i] - '0');
		if (n > limit / 10 || (n == limit / 10 && d > limit % 10))
			return (0);
		n = n * 10 + d;
		i++;
	}
	return (1);
}

static int	start_index(const char *s, int *sign)
{
	if (s[0] == '+' || s[0] == '-')
	{
		if (s[0] == '-')
			*sign = -1;
		return (1);
	}
	return (0);
}

int	is_within_long_long(const char *s)
{
	size_t	i;
	int		sign;
	unsigned long long	limit;

	sign = 1;
	i = start_index(s, &sign);
	limit = get_limit(sign);
	return (process_digits(s, i, limit));
}
