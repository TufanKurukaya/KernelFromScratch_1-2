#ifndef UTILS_H
# define UTILS_H
# include <stddef.h>
# include <stdint.h>

int		isalpha(int c);
int		isascii(int c);
int		isdigit(int c);
int		isprint(int c);
int		isalnum(int c);
int		strcmp(char *dst, char *src);
void	trim(char *out, char *in);
void	*memmove(void *dst, const void *src, size_t len);
void	*memcpy(void *dst, const void *src, size_t n);
void	*memset(void *s, int c, size_t n);

#endif