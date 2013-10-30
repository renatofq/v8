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
  \file list.h

  \brief Define interface de V8List.
*/

#ifndef V8_LIST_H
#define V8_LIST_H

/*!
  \struct V8List

  \brief Lista simples de valores.
*/
typedef struct v8_list_t V8List;

/*!
  \struct V8ListIterator

  \brief Usado para percorrer a lista.
*/
typedef struct v8_list_node_t V8ListIterator;

/*!
  \fn V8ListAllocator

  \brief Assinatura da funcao responsavel por alocar um novo valor.
  \param Ponteiro para o valor armazenado na lista.
*/
typedef void * (*V8ListAllocator)(const void *);

/*!
  \fn V8ListDestructor

  \brief Assinatura da funcao responsavel por desalocar um valor.
  \param Ponteiro para o valor a ser desalocado,
*/
typedef void (*V8ListDestructor)(void *);

/*!
  \fn V8ListVisitor

  \brief Assinatura da funcao responsavel por visitar um valor.
  \param Ponteiro para o valor.
*/
typedef void (*V8ListVisitor)(const void * value);


/*!
  \brief Cria uma lista.

  \param alocator Ponteiro para o allocador de valor.
  \param destructor Ponteiro para o destrutor de valor.

  \return Ponteiro para nova lista ou NULL em caso de erro.

  Os parametros allocator e destructor podem ser NULL ou devem ser funcoes
  responsaveis por duplicar e destruir, respectivamente, os valores inseridos na
  lista. Os valores de allocator e destructor sao independentes, isto e, um
  allocator NULL nao impilca em um destructor NULL e vice versa.

  Quando allocator e NULL o valor inserido em v8_list_insert e copiado por
  como e, ou seja, copia apenas o ponteiro. Ressalta-se que esse valor deve ser
  persistente durante toda a vida da lista. Quando allocator nao e NULL a funcao
  apontada por allocator e chamada para criar uma copia persistente de valor.

  Quando destructor e NULL fica a cargo  do cliente garantir que os recursos
  consumidos pelo valor sejam liberados, se for o caso. Quando destruct nao e
  NULL, destructor e chamado passando o valor a ser desalocado em v8_list_remove
  e quando da destruicao da lista. Ressalta-se se destructor nao for  NULL, os
  todos os valores inseridos na lista serao invalidos apos a chamada de
  v8_list_destroy.
*/
V8List * v8_list_create(V8ListAllocator allocator, V8ListDestructor destructor);

/*!
  \brief Libera os recursos utilizados pela lista
  \param list Lista.
  \see v8_list_create
*/
void v8_list_destroy(V8List * list);

/*!
  \brief  Insere um valor no inicion da lista
  \param list Lista.
  \param data valor a ser inserido.
  \return 0 em caso de erro, 1 do contrario.
  \see v8_list_create
*/
int v8_list_push(V8List * list, const void  * data);

/*!
  \brief Remove um elemento do inicio da lista e retorna seu valor.
  \param list Lista.
  \return Valor do elemento removido.
*/
const void * v8_list_pop(V8List * list);

/*!
  \brief Remove membro da lista.

  \param list Lista.
  \param it elemento a ser removido.

  \see v8_list_create
*/
void v8_list_remove(V8List * list, V8ListIterator * it);

/*!
  \brief Itera a lista.

  \param list Lista.
  \param visitor funcao chamada por elemento.
*/
void v8_list_iterate(const V8List * list, V8ListVisitor visitor);

/*!
  \brief Retorna o iterador do primeiro elemento da lista.

  \param list Lista.
  \return Iterador para o primeiro elemento ou NULL caso a lista seja vazia.
*/
const V8ListIterator * v8_list_iterator(const V8List * list);

/*!
  \brief Retorna itetador para o proximo membro da list.

  \param it Iterador.
  \return Iterador para o proximo elemento ou NULL caso a lista seja vazia.
*/
const V8ListIterator * v8_list_iterator_next(const V8ListIterator * it);

/*!
  \brief Retorna o valor do iterador.

  \param it Iterador.
  \return Valor contido.
*/
const void * v8_list_iterator_get(const V8ListIterator * it);

#endif
