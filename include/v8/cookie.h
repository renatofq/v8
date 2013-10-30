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

/*!
  \file  cookie.h

  \brief Interface de V8Cookie.
  \todo Formatacao de cookie imprime apenas nome=valor, ignorando todo o resto.
  \todo Criar funcoes para limpar os parametros de cookie.
*/

#ifndef V8_COOKIE_H
#define V8_COOKIE_H

#include <time.h>

/*!
  \struct V8Cookie

  \brief Representa um Cookie HTTP.
*/
typedef struct v8_cookie_t V8Cookie;

/*!
  \brief Cria um V8Cookie

  \param name Nome do cookie HTTP.
  \param value Valor do cookie.
  \return Ponteiro para V8Cookie ou NULL em caso de erro.
*/
V8Cookie * v8_cookie_create(const char * name, const char * value);

/*!
  \brief Libera os recursos de V8Cookie.

  \param cookie V8Cookie a ser destruido.
*/
void v8_cookie_destroy(V8Cookie * cookie);

/*!
  \brief Trascreve cookie para o formato string conforme o HTTP.

  \param cookie Cookie a ser transcrito.
  \param buffer Ponteiro para a string a ser escrita.
  \param size Tamanho de buffer.

  \return Numero de bytes escritos em buffer.
*/
int v8_cookie_print(const V8Cookie * cookie, char * buffer, long size);

/*!
  \brief Retorna o nome do cookie

  \param cookie Cookie.
  \return Nome do cookie.
*/
const char * v8_cookie_name(const V8Cookie * cookie);

/*!
  \brief Retorna o valor do cookie.

  \param cookie Cookie.
  \return Valor do cookie.
*/
const char * v8_cookie_value(const V8Cookie * cookie);

/*!
  \brief Define valor do cookie.

  \param cookie Cookie.
  \param value Novo valor.
*/
void v8_cookie_set_value(V8Cookie * cookie, const char * value);

/*!
  \brief Retorna o *domain* do cookie.

  \param cookie Cookie.
  \return *Domain* do cookie.
*/
const char * v8_cookie_domain(const V8Cookie * cookie);

/*!
  \brief Define o *domain* do cookie.

  \param cookie Cookie.
  \param domain *Domain* do cookie.
*/
void v8_cookie_set_domain(V8Cookie * cookie, const char * value);

/*!
  \brief Retorna o *path* do cookie.

  \param cookie Cookie.
  \return *Path* do cookie.
*/
const char * v8_cookie_path(const V8Cookie * cookie);

/*!
  \brief Define *path* do cookie.

  \param cookie Cookie.
  \param value *Path* do cookie.
*/
void v8_cookie_set_path(V8Cookie * cookie, const char * value);

/*!
  \brief Retorna a validade do cookie.

  \param cookie Cookie.
  \return Data de validade do cookie.
 */
time_t v8_cookie_expires(const V8Cookie * cookie);

/*!
  \brief Define a validade do cookie.

  \param cookie Cookie.
  \param expires Data de validade do cookie.
*/
void v8_cookie_set_expires(V8Cookie * cookie, time_t expires);

/*!
  \brief Testa se *Secure* esta definido

  \param cookie Cookie.
  \return diferente de 0 se *Secure* estive definido, 0 do contrario.
*/
int v8_cookie_is_secure(const V8Cookie * cookie);

/*!
  \brief Habilita *Secure*.

  \param cookie Cookie.
*/
void v8_cookie_set_secure(V8Cookie * cookie);

/*!
  \brief Testa se *HttpOnly* esta definido.

  \param cookie Cookie.
  \return diferente de 0 se *HttpOnly* estive definido, 0 do contrario.
*/
int v8_cookie_is_http_only(const V8Cookie * cookie);

/*!
  \brief Habilita *HttpOnly*.

  \param cookie Cookie.
*/
void v8_cookie_set_http_only(V8Cookie * cookie);

#endif
