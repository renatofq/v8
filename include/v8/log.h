#ifndef V8_LOG_H
#define V8_LOG_H

#define v8_log_info(FMT, ...) (v8_log("[INFO] " FMT "\n", __VA_ARGS__))
#define v8_log_warn(FMT, ...) v8_log("[WARN] " FMT "\n", __VA_ARGS__)
#define v8_log_error(FMT, ...) v8_log("[ERROR] " FMT "\n", __VA_ARGS__)

#ifdef V8_DEBUG
#define v8_log_debug(FMT, ...) v8_log("[DEBUG] " FMT "\n", __VA_ARGS__)
#else
#define v8_log_debug(FMT, ...)
#endif

void v8_log(const char * fmt, ...);

#endif
