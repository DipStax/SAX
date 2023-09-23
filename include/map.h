#ifndef MAP_H__
#define MAP_H__

#include <stdbool.h>

#include "utils.h"

#define MAP_PARAM(_type, _name, _value) \
     _type *_name = alloc_zero(sizeof(_type)); \
    (*_name) = _value;

typedef struct node_pair_s
{
    struct node_pair_s *next;
    struct node_pair_s *prev;
    void *key;
    void *value;
} node_pair_t;

node_pair_t *node_pair_create(void *_key, void *_value);
void node_pair_destroy(node_pair_t *_node);

typedef bool (*fn_map_cmp)(void *, void *);
// Second param:
// true = key
// false = value
typedef void (*fn_map_del)(void *, bool);

node_pair_t *priv_node_pair_find(node_pair_t *_head, void *_comp, fn_map_cmp _fn_cmp);
void priv_node_pair_clear(node_pair_t *_head, fn_map_del _fn_del);

typedef struct map_s
{
    node_pair_t *list;
    size_t size;
    fn_map_cmp __cmp;
    fn_map_del __del;
} map_t;

map_t *map_create(fn_map_cmp _cmp, fn_map_del _del);
void map_destroy(map_t *_map);

node_pair_t *map_push(map_t *_map, void *_key, void *_value);

void *map_at(map_t *_map, void *_cmp);
node_pair_t *map_find(map_t *_map, void *_cmp);

void map_erase(map_t *_map, void *_value);
void map_clear(map_t *_map);

void map_set_cmp(map_t *_map, fn_map_cmp _fn_cmp);
void map_set_del(map_t *_map, fn_map_del _fn_del);

#endif