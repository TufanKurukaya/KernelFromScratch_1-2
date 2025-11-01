#include <stddef.h>
#include <stdint.h>

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

void	trim(char *out, char *in)
{
	int	i;
	int	x;

	i = 0;
	x = 0;
	while (in[i])
	{
		if (in[i] > 32 && in[i] <= 126)
		{
			out[x++] = in[i];
			if (in[i + 1] == ' ')
				out[x++] = ' ';
		}
		i++;
	}
	if ((x > 0) && out[x - 1] == ' ')
		out[x - 1] = '\0';
	out[x] = '\0';
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

void	*memset(void *s, int c, size_t n)
{
	unsigned char	*p;

	p = s;
	while (n-- > 0)
	{
		*p++ = (unsigned char)c;
	}
	return (s);
}