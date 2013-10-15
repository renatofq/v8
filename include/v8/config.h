#ifndef V8_CONFIG_H
#define V8_CONFIG_H

typedef struct v8_config_t V8Config;

V8Config * v8_config_create(void);

V8Config * v8_config_create_from_file(const char * file);

void v8_config_destroy(V8Config * config);

const char * v8_config_str(const V8Config * config, const char * key,
                           const char * def);

void v8_config_set_str(const V8Config * config, const char * key,
                       const char * val);


int v8_config_int(const V8Config * config, const char * key, const int def);

void v8_config_set_int(const V8Config * config, const char * key, const int val);

#endif
