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

#include <v8/config.h>
#include <v8/strmap.h>
#include <v8/log.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define V8_CONFIG_BUFFER_SIZE (1024)

struct v8_config_t
{
	V8Map * map;
};

V8Config * v8_config_create(void)
{
	V8Config * config = malloc(sizeof(V8Config));

	if (config != NULL)
	{
		config->map = v8_strmap_create();
		if (config->map == NULL)
		{
			v8_config_destroy(config);
			config = NULL;
		}
	}

	return config;
}

V8Config * v8_config_create_from_file(const char * filename)
{
	char line[V8_CONFIG_BUFFER_SIZE];

  /* !!! WARNING !!!
     When modifying the size of key and/or val arrays ensure that the
     scanfs calls are updated accordingly */
	char key[256];
	char val[256];

	char * str = NULL;
	FILE * file = NULL;
	V8Config * config = v8_config_create();
	int ret = 0;
	int len = 0;

	if (config == NULL)
  {
	  goto error_cleanup;
  }

	file = fopen(filename, "r");
	if (file == NULL)
	{
		goto error_cleanup;
	}

	while(feof(file) == 0 && ferror(file) == 0)
  {
	  str = fgets(line, V8_CONFIG_BUFFER_SIZE, file);
	  if (str == NULL)
		{
			continue;
		}

	  len = strlen(line);
	  if (len <= 1)
	  {
		  /* line is empty */
		  continue;
		}

	  /* drop line break */
	  --len;
	  line[len] = '\0';

	  /* Drop comments */
	  str = strchr(line, '#');
	  if (str != NULL)
		{
			*str = '\0';
			str = NULL;
		}

	  str = strchr(line, ';');
	  if (str != NULL)
	  {
		  *str = '\0';
		  str = NULL;
	  }

	  /* scan config */
	  ret = sscanf(line, " %255s = %255s ", key, val);
	  if (ret == 1)
		{
			v8_log_warn("Configuration file ill formed at line %s", line);
			break;
		}
	  else if (ret != 2)
		{
			/* line possibly has only blanks */
			continue;
		}

	  v8_strmap_insert(config->map, key, val);
  }

	if (ferror(file) != 0)
	{
		goto error_cleanup;
	}

	fclose(file);

	return config;

 error_cleanup:
	if (file != NULL)
  {
	  fclose(file);
	  file = NULL;
  }

	if (config != NULL)
	{
		v8_config_destroy(config);
		config = NULL;
	}
	return NULL;
}

void v8_config_destroy(V8Config * config)
{
	if (config == NULL)
  {
	  return;
  }

	if (config->map != NULL)
  {
	  v8_map_destroy(config->map);
	  config->map = NULL;
  }

	free(config);
}

const char * v8_config_str(const V8Config * config, const char * key,
                           const char * def)
{
	const char * val = NULL;

	if (config == NULL || key == NULL)
	{
		return def;
	}

	val = v8_strmap_value(config->map, key);
	if (strlen(val) == 0)
	{
		v8_log_info("Config: %s not found. Fallback to default %s", key, def);
		return def;
	}
	else
	{
		return val;
	}
}

void v8_config_set_str(V8Config * config, const char * key,
                       const char * val)
{
	if (config == NULL || key == NULL)
	{
		return;
	}

	v8_strmap_insert(config->map, key, val);
}


int v8_config_int(const V8Config * config, const char * key, const int def)
{
	const char * val = NULL;

	if (config == NULL || key == NULL)
	{
		return def;
	}

	val = v8_strmap_value(config->map, key);
	if (strlen(val) == 0)
	{
		v8_log_info("Config: %s not found. Fallback to default %d", key, def);
		return def;
	}
	else
	{
		return atoi(val);
	}

}

void v8_config_set_int(V8Config * config, const char * key, const int val)
{
	char str[12];

	if (config == NULL || key == NULL)
	{
		return;
	}

	snprintf(str, sizeof(str), "%d", val);
	v8_strmap_insert(config->map, key, str);
}

#undef V8_CONFIG_BUFFER_SIZE
