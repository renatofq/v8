#include <v8/config.h>
#include <v8/strmap.h>

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
	V8Config * config = (V8Config *)malloc(sizeof(V8Config));

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
  /* !!! ATENCAO !!!
     Ao modificar o tamaho dos arrays key e val certifique-se
     de atualizar os scanfs onde sao usados ao longo da funcao */
	char key[256];
	char val[256];
	char * str = NULL;
	FILE * file = NULL;
	int bad_format = 0;
	V8Config * config = v8_config_create();


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

	  /* Descarta comentarios e quebra de linha */
	  strtok(line, "\n#;");

	  if (sscanf(line, "%255[a-zA-Z.] = %255[a-zA-Z0-9]", key, val) != 2)
		{
			/* malformatado */
			bad_format = 1;
			break;
		}

	  v8_strmap_insert(config->map, key, val);
  }

	if (bad_format != 0)
  {
	  goto error_cleanup;
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
	if (val == NULL)
	{
		return def;
	}
	else
	{
		return val;
	}
}

void v8_config_set_str(const V8Config * config, const char * key,
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
	if (val == NULL)
	{
		return def;
	}
	else
	{
		return atoi(val);
	}

}

void v8_config_set_int(const V8Config * config, const char * key, const int val)
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
