extern char command[81];
#include <stdint.h>
#include <stddef.h>
int	isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	return (0);
}

int	isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	return (0);
}

int	isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int	isprint(int c)
{
	if (c >= 32 && c <= 126)
		return (1);
	return (0);
}

int	isalnum(int c)
{
	return (isalpha(c) || isdigit(c));
}

void	trim(char *s)
{
	int i = 0;
	int x = 0;

	while (s[i])
	{
		if (s[i] > 32 && s[i] <= 126)
		{
			command[x++] = s[i];
			if (s[i + 1] == ' ')
				command[x++] = ' ';
		}
		i++;
	}
	if ((x > 0) && command[x - 1] == ' ')
		command[x - 1] = '\0';
	command[x] = '\0';
}

void	*memcpy(void *dst, const void *src, size_t n)
{
	size_t		i;
	char		*d;
	const char	*s;

	i = 0;
	d = dst;
	s = src;
	if (!src && !dst)
		return (NULL);
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return ((void *)d);
}

void	*memmove(void *dst, const void *src, size_t len)
{
	char	*d;
	char	*s;

	s = (char *)src;
	d = (char *)dst;
	if (!dst && !src)
		return (NULL);
	if (d > s)
	{
		while (len--)
			d[len] = s[len];
	}
	else
		return (memcpy(dst, src, len));
	return ((void *)d);
}