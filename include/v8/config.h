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
  \file config.h
  \brief Responsavel por ler o arquivo de configuracao e disponibilizar os dados
         a aplicacao.
*/

#ifndef V8_CONFIG_H
#define V8_CONFIG_H

/*!
  \struct V8Config
  \brief Representa os dados de confiuracao.
*/
typedef struct v8_config_t V8Config;

/*!
  \brief Cria um V8Config.

  \return Ponteiro para V8Config ou NULL em caso de erro.
*/
V8Config * v8_config_create(void);

/*!
  \brief Cria um V8Config a partir de um arquivo de configuracao valido.

  \param file Caminho para o arquivo de configuracao.
  \return Ponteiro para V8Config ou NULL em caso de erro.

  \warning Nao e reentrante.
*/
V8Config * v8_config_create_from_file(const char * file);

/*!
  \brief Libera os recursos de V8Config.

  \param config V8Config a ser liberado
*/
void v8_config_destroy(V8Config * config);

/*!
  \brief Busca o valor string de uma configuracao.

  \param config Configuracoes a serem buscadas.
  \param key Nome da configuracao a ser buscada.
  \param def Valor *default* da configuracao.

  \return retorna o valor da configuracao em formato string ou o valor *default*
          caso nao encontre.
*/
const char * v8_config_str(const V8Config * config, const char * key,
                           const char * def);

/*!
  \brief Define um valor string para uma configuracao. Caso o valor ainda nao
         exista, adiciona, caso contrario, sobreescreve.

  \param config Configuracoes a serem modificadas
  \param key Nome da configuracao a ser definida.
  \param val Novo valor.
*/

void v8_config_set_str(V8Config * config, const char * key,
                       const char * val);

/*!
  \brief Busca o valor inteiro de uma configuracao.

  \param config Configuracoes a serem buscadas.
  \param key Nome da configuracao a ser buscada.
  \param def Valor *default* da configuracao.

  \return retorna o valor da configuracao em formato inteiro ou o valor
          *default* caso nao encontre.
*/
int v8_config_int(const V8Config * config, const char * key, const int def);

/*!
  \brief Define um valor inteiro para uma configuracao. Caso o valor ainda nao
         exista, adiciona, caso contrario, sobreescreve.

  \param config Configuracoes a serem modificadas
  \param key Nome da configuracao a ser definida.
  \param val Novo valor.
*/
void v8_config_set_int(V8Config * config, const char * key, const int val);

#endif
