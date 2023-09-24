#ifndef LIST_H__
#define LIST_H__

#include <stddef.h>

typedef struct list_it_s
{
    void *value;
    struct list_it_s *next;
    struct list_it_s *prev;
} list_it_t;

list_it_t *list_it_create(void *value);
void list_it_destroy(list_it_t *_it);

typedef void (*fn_list_del)(void *);

list_it_t *priv_list_it_at(list_it_t *_it, size_t _idx);
void priv_list_it_clear(list_it_t *_it, fn_list_del _fn_del);

typedef struct list_s
{
    size_t size;
    list_it_t *__list;
    list_it_t *__last;
    fn_list_del __del;
} list_t;

list_t *list_create(size_t _size, fn_list_del _del);
void list_destroy(list_t *_list);

size_t list_resize(list_t *_list, size_t _nsize);

void *list_at(list_t *_list, size_t _idx);
void *list_push(list_t *_list, void *_data);

void list_clear(list_t *_list);
void list_erase_at(list_t *_list, size_t _idx);

void list_set_del(list_t *_list, fn_list_del _del);

#endif