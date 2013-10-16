/*!
  \file buffer.h
  \brief Buffer de string que cresce automaticamente.
*/

#ifndef V8_BUFFER_H
#define V8_BUFFER_H

/*!
  \struct V8Buffer

  \brief Dados do buffer.

  Essa estrutura nao deve ser alocada staticamente \see v8_buffer_create. Apos o
  deve ser liberada com \see v8_buffer_destroy.
*/
typedef struct v8_buffer_t V8Buffer;

/*!
  \brief Cria um V8Buffer.

  \return Retorna um ponteiro valido para V8Buffer em caso de sucesso,
          do contrario retorna NULL.
*/
V8Buffer * v8_buffer_create(void);

/*!
  \brief Libera os recursos de V8Buffer.

  \param buffer Ponteiro para o buffer a ser liberado.
*/
void v8_buffer_destroy(V8Buffer * buffer);

/*!
  \brief Retorna o tamanho utilizado do buffer.

  \param buffer Poteiro para o buffer.
  \return Tamanho utilizado do buffer.
*/
int v8_buffer_size(const V8Buffer * buffer);

/*!
  \brief Acrescenta uma string ao final do buffer.

  \param buffer Buffer a ser modificado.
  \param str String a ser concatenada no buffer.
  \return 0 caso str seja vazia ou caso ocorra algum erro, o tamanho de str em
          caso de sucesso.
*/
int v8_buffer_append(V8Buffer * buffer, const char * str);

/*
  \brief Escreve buffer para a *stream* descrita por fd.

  \param buffer Buffer a ser escrito.
  \param fd *file descriptor* da *stream* a qual buffer deve ser escrito.
  \return Quantidade de bytes escritos ou < 0 em caso de erro.
*/
int v8_buffer_dump(const V8Buffer * buffer, int fd);

#endif
