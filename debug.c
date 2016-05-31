#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// int print_debug(const char *format, ...);
// void print_string(char *text, char *var);
// void print_int(char *text, int *var);

int print_debug(const char *format, ...)
{
	va_list arg;
	int done;

	char d_format[1024] = "\033[1;31m[d]\033[1;33m ";
	strcat(d_format, format);
	strcat(d_format, "\033[0m");

	va_start(arg, format);
	done = vfprintf(stderr, d_format, arg);
	va_end(arg);

	return done;
}

void print_string(char *text, char *var)
{
	print_debug("%-20s: %15p => '%s' (char *)\n", text, &(var[0]), var);
}

void print_int(char *text, int *var)
{
	print_debug("%-20s: %15p => '%i' (int)\n", text, var, *var);
}

void print_done()
{
	print_debug("DONE.\n");
}
