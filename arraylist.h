#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ARRAYLIST_CHUNK_SIZE 16

typedef struct arraylist{
    void *values;
    unsigned type;
    size_t size;
    size_t capacity;
    void (*handler)(void *);
}arraylist;

// Foreach

#define arraylist_foreach(item, list) \
    _arraylist_foreach(item, (list), _iterator_##item)

#define _arraylist_foreach(item, list, i)  \
    for(                                   \
        void *item = list->values, *i = 0; \
        (size_t) i++ < list->size;         \
        item += list->type                 \
    )

// Reverse foreach

#define arraylist_reverse_foreach(item, list) \
    _arraylist_reverse_foreach(item, (list), _iterator_##item)

#define _arraylist_reverse_foreach(item, list, i)                          \
    for(                                                                   \
        void *item = list->values + (list->size - 1) * list->type, *i = 0; \
        (size_t) i++ < list->size;                                         \
        item -= list->type                                                 \
    )

/**
 * Creates a new arraylist
 *
 * @param type size of the type to contain
 *
 * @return ponter to the new arraylist
**/

#define arraylist_create(type) \
    _arraylist_create(sizeof(type))

arraylist *_arraylist_create(unsigned type){
    arraylist *list = (arraylist *) malloc(sizeof(arraylist));

    list->values = malloc(type * ARRAYLIST_CHUNK_SIZE);
    list->type = type;
    list->size = 0;
    list->capacity = ARRAYLIST_CHUNK_SIZE;
    list->handler = NULL;

    return list;
}

/**
 * Creates a new arraylist with custom initial size
 *
 * @param type size of the type to contain
 * @param size initial size
 *
 * @return ponter to the new arraylist
**/

#define arraylist_create_size(type, size) \
    _arraylist_create_size(sizeof(type), size)

arraylist *_arraylist_create_size(unsigned type, size_t size){
    arraylist *list = (arraylist *) malloc(sizeof(arraylist));

    list->values = malloc(type * size);
    list->type = type;
    list->size = 0;
    list->capacity = ARRAYLIST_CHUNK_SIZE;
    list->handler = NULL;

    return list;
}

/**
 * Reallocate the values array if is necessary
 *
 * @param list arraylist to resize
**/

void arraylist_allocate(arraylist *list){
    if(list->size++ < list->capacity)
        return;

    list->capacity += ARRAYLIST_CHUNK_SIZE;
    list->values = realloc(list->values, list->capacity * list->type);
}

/**
 * Reallocate the values array if is necessary
 *
 * @param list arraylist to resize
 * @param size size to resize
**/

void arraylist_allocate_all(arraylist *list, size_t size){
    list->size += size;

    if(list->size <= list->capacity)
        return;

    unsigned remainder = list->size % ARRAYLIST_CHUNK_SIZE;

    list->capacity = list->size + (remainder ? ARRAYLIST_CHUNK_SIZE - remainder : 0);
    list->values = realloc(list->values, list->capacity * list->type);
}

/*
 * Resize the arraylist to fit with the values size
 *
 * @param list arraylist to resize
**/

void arraylist_optimize(arraylist *list){
    if (list->capacity < list->size + ARRAYLIST_CHUNK_SIZE)
        return;
    
    unsigned remainder = list->size % ARRAYLIST_CHUNK_SIZE;

    list->capacity = list->size + (remainder ? ARRAYLIST_CHUNK_SIZE - remainder : 0);
    list->values = realloc(list->values, list->capacity * list->type);
}

/**
 * Set a handler to the arraylist
 *
 * @param list arraylist to set the handler
 * @param new_handler pointer to the handler
**/

#define arraylist_set_handler(list, new_handler) \
    (list)->handler = (void *)(new_handler)

/**
 * Remove the handler from the arraylist
 *
 * @param list arraylist to remove the handler
**/

#define arraylist_remove_handler(list) \
    (list)->handler = NULL

/**
 * Add an element to the end of the arraylist
 *
 * @param list arraylist to store the value
 * @param value pointer to the value to store
**/

#define arraylist_add(list, value) \
    arraylist_add_index(list, value, (list)->size)

/**
 * Add an element to the arraylist based on an index
 *
 * @param list arraylist to store the value
 * @param value pointer to the value to store
 * @param index position where the value will be inserted
**/

#define arraylist_add_index(list, value, index)    \
    do {                                           \
      typeof(value) _value = value;                \
      _arraylist_add_index(list, &_value, index);  \
    } while (0)

void _arraylist_add_index(arraylist *list, void *value, size_t index){
    arraylist_allocate(list);

    void *zone_to_move = list->values + index * list->type;

    memmove(
        zone_to_move + list->type,
        zone_to_move,
        list->type * (list->size - index - 1)
    );

    memcpy(
        zone_to_move,
        value,
        list->type
    );
}

/**
 * Add an array of elements to the end of the arraylist
 *
 * @param list arraylist to store the values
 * @param argc values amount
 * @param values values to store
**/

#define arraylist_add_all(list, argc, values) \
    arraylist_add_all_index(list, argc, values, (list)->size)

/**
 * Add an element to the arraylist based on an index
 *
 * @param list arraylist to store the value
 * @param argc number of values
 * @param values pointer to the array with the values
 * @param index position where the values will be inserted
**/

void arraylist_add_all_index(arraylist *list, size_t argc, void *values, size_t index){
    arraylist_allocate_all(list, argc);

    void *zone_to_move = list->values + index * list->type;

    memmove(
        zone_to_move + argc * list->type,
        zone_to_move,
        list->type * (list->size - index - argc)
    );

    memcpy(
        zone_to_move,
        values,
        list->type * argc
    );
}

/**
 * Return a pointer to a value based in an index
 *
 * @param list arraylist to get the value
 * @param index position of the value
 *
 * @return a pointer to the value
**/

#define arraylist_get(list, index) \
    ((list)->values + ((list)->type * (index)))

/**
 * Delete an elements of the arraylist
 *
 * @param list arraylist to remove the value
 * @param index position of the value to remove
**/

void arraylist_remove(arraylist *list, size_t index){
    if(list->handler)
        (*(list->handler))(*(void **)arraylist_get(list, index));

    list->size--;
    void *zone_to_move = list->values + index * list->type;
    
    memcpy(
        zone_to_move,
        zone_to_move + list->type,
        (list->size - index) * list->type
    );
}

/**
 * Delete a range of elements of the arraylist
 *
 * @param list arraylist to remove the values
 * @param start start of the range to remove
 * @param end end of the range to remove
**/

void arraylist_remove_range(arraylist *list, size_t start, size_t end){
    end++;
    
    if(list->handler)
        for (size_t i = start; i < end; i++)
            (*(list->handler))(*(void **)arraylist_get(list, i));

    memcpy(
        list->values + start * list->type,
        list->values + end * list->type,
        (list->size - end) * list->type
    );

    list->size -= (end - start);
}

/**
 * Return a slice of the arraylist
 *
 * @param src arraylist to get the slice
 * @param index index to start to copy from
 * @param lenght amount to copy
 *
 * @return slice of the arraylist
**/

arraylist *arraylist_slice(arraylist *src, size_t index, size_t length){
    arraylist *dest = (arraylist *) malloc(sizeof(arraylist));

    dest->type = src->type;
    dest->size = length;
    dest->handler = src->handler;

    size_t capacity = length % ARRAYLIST_CHUNK_SIZE;
    dest->capacity = src->size + (capacity ? ARRAYLIST_CHUNK_SIZE - capacity : 0);

    dest->values = malloc(dest->type * dest->capacity);

    memcpy(
        dest->values,
        src->values + index * src->type,
        dest->type * length
    );

    return dest;
}

/**
 * Return a clone of the arraylist
 *
 * @param list arraylist to clone
 *
 * @return clone of the arraylist
**/

#define arraylist_clone(list) \
	arraylist_slice(list, 0, (list)->size)

/**
 * Insert an arraylist into another at the given index
 *
 * @param dest arraylist to insert the other
 * @param src arraylist to copy
 * @param index position in which the arraylist will be inserted
**/

#define arraylist_join_index(dest, src, index) \
    arraylist_add_all_index(dest, (src)->size, (src)->values, index)

/**
 * Insert an arraylist into another
 *
 * @param dest arraylist to insert the other
 * @param src arraylist to copy
**/

#define arraylist_join(dest, src) \
	arraylist_join_index(dest, src, (dest)->size)

/**
 * Clear all the values from the arraylist
 *
 * @param list arraylist to clear
**/

void arraylist_clear(arraylist *list){
    if(list->handler)
        arraylist_foreach(item, list)
            (*(list->handler))(*(void **)item);

    list->size = 0;
}

/**
 * Free the arraylist
 *
 * @param list arraylist to free
**/

void arraylist_free(arraylist *list){
    if(list->handler)
        arraylist_clear(list);
    
    free(list->values);
    free(list);
}

#undef ARRAYLIST_CHUNK_SIZE

