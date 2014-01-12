/* Copyright (C) 2013, Renato Fernandes de Queiroz <renatofq@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 3 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include <v8/log.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static V8LogLevel g_log_level = V8_LOG_NONE;

void v8_log(V8LogLevel level, const char * fmt, ...)
{
	va_list args;
	const char * prefix = NULL;

	if (level > g_log_level)
	{
		return;
	}

#ifdef V8_DEBUG
	switch (level)
	{
	case V8_LOG_ERROR:
		prefix = "[ERROR] ";
		break;
	case V8_LOG_WARN:
		prefix = "[WARN] ";
		break;
	case V8_LOG_INFO:
		prefix = "[INFO] ";
		break;
	case V8_LOG_DEBUG:
		prefix = "[DEBUG] ";
		break;
	default:
		prefix = "";
		break;
	}
#else
	switch (level)
	{
	case V8_LOG_ERROR:
		prefix = "<3>";
		break;
	case V8_LOG_WARN:
		prefix = "<4>";
		break;
	case V8_LOG_INFO:
		prefix = "<6>";
		break;
	default:
		prefix = "<7>";
		break;
	}
#endif


	fprintf(stderr, prefix);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	fflush(stderr);
}


V8LogLevel v8_log_level(void)
{
	return g_log_level;
}

void v8_log_level_set(V8LogLevel level)
{
	g_log_level = level;
}

void v8_log_level_str_set(const char * level)
{
	if (strcasecmp(level, "debug") == 0)
	{
		g_log_level = V8_LOG_DEBUG;
	}
	else if (strcasecmp(level, "info") == 0)
	{
		g_log_level = V8_LOG_INFO;
	}
	else if (strcasecmp(level, "warning") == 0)
	{
		g_log_level = V8_LOG_WARN;
	}
  else if (strcasecmp(level, "warn") == 0)
  {
	  g_log_level = V8_LOG_WARN;
  }
  else if (strcasecmp(level, "error") == 0)
  {
	  g_log_level = V8_LOG_ERROR;
  }
  else if (strcasecmp(level, "none") == 0)
  {
	  g_log_level = V8_LOG_NONE;
  }
  else
  {
	  v8_log_warn("Log level unidentified, setting to info");
	  g_log_level = V8_LOG_INFO;
  }
}
