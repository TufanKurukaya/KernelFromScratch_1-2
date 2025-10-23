extern char command[81];

int	ft_isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	return (0);
}

int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	return (0);
}

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int	ft_isprint(int c)
{
	if (c >= 32 && c <= 126)
		return (1);
	return (0);
}

int	ft_isalnum(int c)
{
	return (ft_isalpha(c) || ft_isdigit(c));
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
