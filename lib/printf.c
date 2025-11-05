#include "printf.h"
#include "utils.h"
void		putchar(char c);

int	print_char(int c)
{
	if (isprint(c) || c == '\n')
		putchar(c);
	return (1);
}

int	print_str(char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (-1);
	while (str[i])
		if (print_char(str[i++]) == -1)
			return (-1);
	return (i);
}

int	print_hex(unsigned long num, char format, int len)
{
	char	hex_result[1024];
	int		i;

	if (format == 'p')
		len += print_str("0x");
	if (!num && len >= 0)
		len += print_char('0');
	i = 0;
	while (num > 0 && len >= 0)
	{
		if (format == 'x' || format == 'p')
			hex_result[i++] = "0123456789abcdef"[num % 16];
		else
			hex_result[i++] = "0123456789ABCDEF"[num % 16];
		num /= 16;
	}
	hex_result[i] = '\0';
	while (i > 0 && len >= 0)
		len += print_char(hex_result[--i]);
	return (len);
}

int	print_int(long num, int len)
{
	char	dec_result[32];
	int		i;

	i = 0;
	if (num < 0)
	{
		len += print_char('-');
		num = -num;
	}
	if (!num)
		len += print_char('0');
	while (num > 0 && len >= 0)
	{
		dec_result[i++] = "0123456789"[num % 10];
		num /= 10;
	}
	dec_result[i] = '\0';
	while (i > 0 && len >= 0)
		len += print_char(dec_result[--i]);
	return (len);
}

int	print_u_int(unsigned long num, int len)
{
	char	dec_result[32];
	int		i;

	i = 0;
	if (!num)
		len += print_char('0');
	while (num > 0 && len >= 0)
	{
		dec_result[i++] = "0123456789"[num % 10];
		num /= 10;
	}
	dec_result[i] = '\0';
	while (i > 0 && len >= 0)
		len += print_char(dec_result[--i]);
	return (len);
}

static int	check(char form)
{
	if (form == 'c' || form == 'd' || form == 'i' || form == 'u' || form == '%'
		|| form == 's' || form == 'x' || form == 'X' || form == 'p')
		return (1);
	return (0);
}

static int	formater(va_list *arg, char format)
{
	if (format == 'c')
		return (print_char(va_arg(*arg, int)));
	else if (format == 's')
		return (print_str(va_arg(*arg, char *)));
	else if (format == 'p')
		return (print_hex(va_arg(*arg, unsigned long), format, 0));
	else if (format == 'd' || format == 'i')
		return (print_int(va_arg(*arg, int), 0));
	else if (format == 'u')
		return (print_u_int(va_arg(*arg, unsigned int), 0));
	else if (format == 'x' || format == 'X')
		return (print_hex(va_arg(*arg, unsigned int), format, 0));
	else if (format == '%')
		return (print_char('%'));
	else
		return (0);
}

int	printf(const char *format, ...)
{
	va_list	arg;
	int		len;
	int		i;
	int		temp;

	len = 0;
	i = -1;
	va_start(arg, format);
	while (format[++i])
	{
		if (format[i] == '%' && check(format[i + 1]))
		{
			temp = formater(&arg, format[++i]);
			if (temp == -1)
				return (-1);
			len += temp;
		}
		else if (format[i] != '%' && print_char(format[i]) == -1)
			return (-1);
		else if (format[i] != '%')
			len++;
	}
	va_end(arg);
	return (len);
}
