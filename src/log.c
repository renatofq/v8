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


	/* FIXME: logs can potentially be messed up due concurrency */
	printf("%s", prefix);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
	printf("\n");
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
