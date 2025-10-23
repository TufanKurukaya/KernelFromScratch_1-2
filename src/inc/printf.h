// src/inc/printf.h
#ifndef PRINTF_H
# define PRINTF_H

# include <stddef.h>
# include <stdint.h>

// GCC built-in va_list support for kernel
typedef __builtin_va_list	va_list;
# define va_start(ap, last) __builtin_va_start(ap, last)
# define va_arg(ap, type) __builtin_va_arg(ap, type)
# define va_end(ap) __builtin_va_end(ap)

int							printf(const char *format, ...);

#endif
