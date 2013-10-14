#ifndef V8_COOKIE_H
#define V8_COOKIE_H

typedef struct v8_cookie_t V8Cookie;

V8Cookie * v8_cookie_create(const char * name, const char * value);
void v8_cookie_destroy(V8Cookie * cookie);

int v8_cookie_print(const V8Cookie * cookie, char * buffer, int size);

const char * v8_cookie_name(const V8Cookie * cookie);
const char * v8_cookie_value(const V8Cookie * cookie);
void v8_cookie_set_value(V8Cookie * cookie, const char * value);

const char * v8_cookie_domain(const V8Cookie * cookie);
void v8_cookie_set_domain(V8Cookie * cookie, const char * value);

const char * v8_cookie_path(const V8Cookie * cookie);
void v8_cookie_set_path(V8Cookie * cookie, const char * value);

int v8_cookie_is_secure(const V8Cookie * cookie);
void v8_cookie_set_secure(V8Cookie * cookie);

int v8_cookie_is_http_only(const V8Cookie * cookie);
void v8_cookie_set_http_only(V8Cookie * cookie);

#endif
