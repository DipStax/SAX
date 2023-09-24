#include <stdlib.h>

#include "utils.h"
#include "map.h"

node_pair_t *node_pair_create(void *_key, void *_value)
{
    node_pair_t *pair = alloc_zero(sizeof(node_pair_t));

    pair->key = _key;
    pair->value = _value;
    return pair;
}

void node_pair_destroy(node_pair_t *_node)
{
    if (_node->prev)
        _node->prev->next = _node->next;
    if (_node->next)
        _node->next->prev = _node->prev;
    free(_node);
}

node_pair_t *priv_node_pair_find(node_pair_t *_head, void *_cmp, fn_map_cmp _fn_cmp)
{
    node_pair_t *node = _head;

    for (; node != NULL; node = node->next)
        if (_fn_cmp(_cmp, node->key))
            return node;
    return NULL;
}

void priv_node_pair_clear(node_pair_t *_head, fn_map_del _fn_del)
{
    if (_head != NULL) {
        priv_node_pair_clear(_head->next, _fn_del);
        _fn_del(_head->key, true);
        _fn_del(_head->value, false);
        free(_head);
    }
}


map_t *map_create(fn_map_cmp _cmp, fn_map_del _fn_del)
{
    map_t *map = alloc_zero(sizeof(map_t));

    map_set_cmp(map, _cmp);
    map_set_del(map, _fn_del);
    return map;
}

void map_destroy(map_t *_map)
{
    map_clear(_map);
    free(_map);
}

node_pair_t *map_push(map_t *_map, void *_key, void *_value)
{
    node_pair_t *node = map_find(_map, _key);

    if (node) {
        _map->__del(node->value, false);
        node->value = _value;
    } else {
        node = node_pair_create(_key, _value);
        if (_map->list) {
            _map->list->prev = node;
            node->next = _map->list;
        }
        _map->list = node;
    }
    _map->size++;
    return node;
}

void *map_at(map_t *_map, void *_cmp)
{
    return map_find(_map, _cmp)->value;
}

node_pair_t *map_find(map_t *_map, void *_cmp)
{
    return priv_node_pair_find(_map->list, _cmp, _map->__cmp);
}

void map_erase(map_t *_map, void *_cmp)
{
    node_pair_t *node = map_find(_map, _cmp);

    if (node == _map->list)
        _map->list = node->next;
    node_pair_destroy(node);
    _map->size--;
}

void map_clear(map_t *_map)
{
    priv_node_pair_clear(_map->list, _map->__del);
    _map->size = 0;
}

void map_set_cmp(map_t *_map, fn_map_cmp _fn_cmp)
{
    _map->__cmp = _fn_cmp;
}

void map_set_del(map_t *_map, fn_map_del _fn_del)
{
    _map->__del = _fn_del;
}