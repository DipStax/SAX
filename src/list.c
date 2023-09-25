#include <stdlib.h>

#include "list.h"
#include "utils.h"

list_it_t *list_it_create(void *_value)
{
    list_it_t *it = alloc_zero(sizeof(list_it_t));

    it->value = _value;
    return it;
}

void list_it_destroy(list_it_t *_it)
{
    if (_it->prev)
        _it->prev->next = _it->next;
    if (_it->next)
        _it->next->prev = _it->prev;
    free(_it);
}

list_it_t *priv_list_it_at(list_it_t *_it, size_t _idx)
{
    list_it_t *it = _it;
    size_t idx = 0;

    for (; it; it = it->next)
        if (idx++ == _idx)
            break;
    return it;
}

void priv_list_it_clear(list_it_t *_it, fn_list_del _fn_del)
{
    if (_it != NULL) {
        priv_list_it_clear(_it->next, _fn_del);
        _fn_del(_it->value);
        free(_it);
    }
}

list_t *list_create(size_t _size, fn_list_del _del)
{
    list_t *list = alloc_zero(sizeof(list_t));

    list_resize(list, _size);
    list_set_del(list, _del);
    return list;
}

void list_destroy(list_t *_list)
{
    list_clear(_list);
    free(_list);
}

size_t list_resize(list_t *_list, size_t _nsize)
{
    size_t modif = 0;

    for (size_t it = _list->size; it < _nsize; it++) {
        list_push(_list, NULL);
        modif++;
    }
    for (size_t it = _list->size; it > _nsize; it++) {
        list_erase_at(_list, _list->size - 1);
        modif--;
    }
    return modif;
}

void *list_at(list_t *_list, size_t _idx)
{
    return priv_list_it_at(_list->__list, _idx)->value;
}

void *list_push(list_t *_list, void *_data)
{
    list_it_t *it = list_it_create(_data);

    if (_list->__list) {
        it->prev = _list->__last;
        _list->__last->next = it;
        _list->__last = it;
    } else {
        _list->__list = it;
        _list->__last = it;
    }
    _list->size++;
    return it;
}

void list_clear(list_t *_list)
{
    priv_list_it_clear(_list->__list, _list->__del);
}

void list_erase_at(list_t *_list, size_t _idx)
{
    list_it_t *it = priv_list_it_at(_list->__list, _idx);

    _list->__del((void *)it);
    list_it_destroy(it);
    _list->size--;
}

void list_set_del(list_t *_list, fn_list_del _del)
{
    _list->__del = _del;
}