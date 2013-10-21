/*!
  \file log.h

  \brief Define interface de V8Log.
*/

#ifndef V8_LOG_H
#define V8_LOG_H

/*!
  \enum V8LogLevel

  \brief Enumera os niveis de log possiveis.
*/
typedef enum v8_log_level_e
{
	V8_LOG_NONE = 0,
	V8_LOG_ERROR,
	V8_LOG_WARN,
	V8_LOG_INFO,
	V8_LOG_DEBUG
} V8LogLevel;

/*!
  \brief Retorna o nivel de log corrente.

  \return Nivel de log corrente.
*/
V8LogLevel v8_log_level(void);

/*!
  \brief Define Nivel de log.

  \param level Nivel de log.
*/
void v8_log_level_set(V8LogLevel level);

/*!
  \brief Define o nivel de log baseado em uma string

  \param level String indica o nivel de log. Valores aceitos:
  {"debug", "info", "warn", "warning", "error", "none"} indiferente
  a capitalizacao.
*/
void v8_log_level_str_set(const char * level);

/*!
  \brief Imprime log.

  \param level Nivel de log.
  \param FMT string de formatacao nos moldes de printf.
  \param args Argumetos contidos em FMT.
*/
void v8_log(V8LogLevel level, const char * fmt, ...);

/*!
  \brief Imprime log nivel INFO.

  \param FMT string de formatacao nos moldes de printf.
  \param args Argumetos contidos em FMT.
*/
#define v8_log_info(...) v8_log(V8_LOG_INFO, __VA_ARGS__)

/*!
  \brief Imprime log nivel WARN.

  \param FMT string de formatacao nos moldes de printf.
  \param args Argumetos contidos em FMT.
*/
#define v8_log_warn(...) v8_log(V8_LOG_WARN,  __VA_ARGS__)

/*!
  \brief Imprime log nivel ERROR.

  \param FMT string de formatacao nos moldes de printf.
  \param args Argumetos contidos em FMT.
*/
#define v8_log_error(...) v8_log (V8_LOG_ERROR, __VA_ARGS__)

#ifdef V8_DEBUG

/*!
  \brief Imprime log nivel DEBUG.

  \param FMT string de formatacao nos moldes de printf.
  \param args Argumetos contidos em FMT.
  \warning Esta funcao so esta definida na compilacao debug.
*/
#define v8_log_debug(...) v8_log(V8_LOG_DEBUG, __VA_ARGS__)
#else
#define v8_log_debug(...)
#endif


#endif
