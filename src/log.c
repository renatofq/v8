#include <v8/log.h>
#include <stdarg.h>
#include <stdio.h>

void v8_log(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
}
