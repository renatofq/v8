#include <v8/list.h>

#include <stdlib.h>

typedef struct v8_list_node_t
{
	const void * data;
	struct v8_list_node_t * next;
} V8ListNode;

struct v8_list_t
{
	V8ListNode * head;
	V8ListAllocator allocator;
	V8ListDestructor destructor;
};


static V8ListNode * v8_list_node_create(const V8List * list, const void * data);
static void v8_list_node_destroy(const V8List * list, V8ListNode * node);

static void v8_list_destroy_r(const V8List * list, V8ListNode * node);
static void v8_list_remove_r(const V8List * list, V8ListNode * node,
                             V8ListIterator * it);
static void v8_list_iterate_r(const V8ListNode * node, V8ListVisitor visitor);

V8List * v8_list_create(V8ListAllocator allocator, V8ListDestructor destructor)
{
	V8List * list = (V8List *)malloc(sizeof(V8List));

	if (list != NULL)
  {
	  list->head = NULL;
	  list->allocator = allocator;
	  list->destructor = destructor;
  }

	return list;
}

void v8_list_destroy(V8List * list)
{
	if (list == NULL)
	{
		return;
	}

	v8_list_destroy_r(list, list->head);

	list->head = NULL;

	free(list);
	list = NULL;
}

int v8_list_insert(V8List * list, const void  * data)
{
	V8ListNode * node = NULL;

	if (list == NULL)
	{
		return 0;
	}

	node = v8_list_node_create(list, data);
	if (node == NULL)
  {
	  return 0;
  }

	node->next = list->head;
	list->head = node;

	return 1;
}

void v8_list_remove(V8List * list, V8ListIterator * it)
{
	if (list == NULL || it == NULL || list->head == NULL)
	{
		return;
	}

	if (list->head == it)
	{
		list->head = list->head->next;
		v8_list_node_destroy(list, it);
	}
	else
  {
	  v8_list_remove_r(list, list->head, it);
  }
}

void v8_list_iterate(const V8List * list, V8ListVisitor visitor)
{
	if (list != NULL && visitor != NULL)
  {
	  v8_list_iterate_r(list->head, visitor);
  }
}

const V8ListIterator * v8_list_iterator(const V8List * list)
{
	if (list == NULL)
	{
		return NULL;
	}
	else
	{
		return list->head;
	}
}

const V8ListIterator * v8_list_iterator_next(const V8ListIterator * it)
{
	if (it == NULL)
	{
		return NULL;
	}
	else
	{
		return it->next;
	}
}

const void * v8_list_iterator_get(const V8ListIterator * it)
{
	if (it == NULL)
  {
	  return NULL;
  }
	else
	{
		return it->data;
	}
}


static V8ListNode * v8_list_node_create(const V8List * list, const void * data)
{
	V8ListNode * node = (V8ListNode *)malloc(sizeof(V8ListNode));

	if (node == NULL)
	{
		return NULL;
	}

	if (list->allocator != NULL)
  {
	  node->data = list->allocator(data);
  }
	else
	{
		node->data = data;
	}

	node->next = NULL;

	return node;
}

static void v8_list_node_destroy(const V8List * list, V8ListNode * node)
{
	if (node == NULL)
	{
		return;
	}

	if (list->destructor != NULL)
	{
		list->destructor((void *)node->data);
	}

	node->data = NULL;
	node->next = NULL;

	free(node);
}

static void v8_list_destroy_r(const V8List * list, V8ListNode * node)
{
	V8ListNode * next = NULL;

	if (node == NULL) return;

	next = node->next;
	v8_list_node_destroy(list, node);

	v8_list_destroy_r(list, next);
}

static void v8_list_remove_r(const V8List * list, V8ListNode * node,
                             V8ListIterator * it)
{
	if (node == NULL || node->next == NULL)
	{
		return;
	}

	if (node->next == it)
	{
		node->next = node->next->next;
		v8_list_node_destroy(list, it);
	}
	else
	{
		v8_list_remove_r(list, node->next, it);
	}
}

static void v8_list_iterate_r(const V8ListNode * node, V8ListVisitor visitor)
{
	if (node != NULL)
	{
		visitor(node);
		v8_list_iterate_r(node->next, visitor);
	}
}
