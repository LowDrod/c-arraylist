#pragma once

#include <stdlib.h>
#include <string.h>

#define ARRAYLIST_CHUNK_SIZE 10

typedef struct arraylist{
    void *values;
    unsigned type;
    size_t size;
    size_t capacity;
    void (*handler)(void *);
}arraylist;

#define arraylist_foreach(list, item) \
    _arraylist_foreach(((arraylist *)list), item)

#define _arraylist_foreach(list, item)                              \
    for(                                                            \
        void *item = list->values,                                  \
            *_last_##item = list->values + list->size * list->type; \
        item != _last_##item;                                       \
        item += list->type                                          \
    )

#define arraylist_reverse_foreach(list, item) \
    _arraylist_reverse_foreach(((arraylist *)list), item)

#define _arraylist_reverse_foreach(list, item)               \
    for(                                                     \
        void *item = list->values + list->size * list->type, \
            *_first_##item = list->values - list->type;      \
        item != _first_##item;                               \
        item -= list->type                                   \
    )

#define arraylist_create(type)      \
    _arraylist_create(sizeof(type))

/**
 * Creates a new arraylist
 *
 * @param type size of the type to contain
 *
 * @return ponter to the new arraylist
**/

arraylist *_arraylist_create(unsigned type){
    arraylist *list = malloc(sizeof(arraylist));

    list->values = malloc(type * ARRAYLIST_CHUNK_SIZE);
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

    list->capacity = (list->size / ARRAYLIST_CHUNK_SIZE + (list->size % ARRAYLIST_CHUNK_SIZE != 0)) * ARRAYLIST_CHUNK_SIZE;
    list->values = realloc(list->values, list->capacity * list->type);
}

/*
 * Resize the arraylist to fit with the values size
 *
 * @param list arraylist to resize
**/

void arraylist_optimize(arraylist *list){
    if (list->capacity == (list->size / ARRAYLIST_CHUNK_SIZE + 1) * ARRAYLIST_CHUNK_SIZE)
        return;

    list->capacity = (list->size / ARRAYLIST_CHUNK_SIZE + (list->size % ARRAYLIST_CHUNK_SIZE != 0)) * ARRAYLIST_CHUNK_SIZE;
    list->values = realloc(list->values, list->capacity * list->type);
}

/**
 * Set a handler to the arraylist
 *
 * @param list arraylist to set the handler
 * @param new_handler pointer to the handler
**/

#define arraylist_set_handler(list, new_handler) \
    list->handler = (void *)new_handler

/**
 * Remove the handler from the arraylist
 *
 * @param list arraylist to remove the handler
**/

#define arraylist_remove_handler(list) \
    list->handler = NULL

/**
 * Add an element to the end of the arraylist
 *
 * @param list arraylist to store the value
 * @param value pointer to the value to store
**/

#define arraylist_add(list, value) \
    _arraylist_add_index(list, &value, list->size)

#define arraylist_add_index(list,  value, index) \
    _arraylist_add_index(list, &value, index)

/**
 * Add an element to the arraylist based on an index
 *
 * @param list arraylist to store the value
 * @param value pointer to the value to store
 * @param index position where the value will be inserted
**/

void _arraylist_add_index(arraylist *list, void *value, size_t index){
    arraylist_allocate(list);

    void *zone_to_move = list->values + index * list->type;

    memmove(
        zone_to_move + list->type,
        zone_to_move,
        list->type * (list->size - index - 1)
    );

    // #include <stdio.h>
    // printf(
    //     "V: %zu => MEM: %zu\n",
    //     *(size_t *)value,
    //     (size_t)((zone_to_move - list->values) / list->type)
    // );

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

#define arraylist_add_all(list, argc, values)                   \
    _arraylist_add_all_index(list, argc, values, list->size)

#define arraylist_add_all_index(list, argc, values, index)      \
    _arraylist_add_all_index(list, argc, values, index)

/**
 * Add an element to the arraylist based on an index
 *
 * @param list arraylist to store the value
 * @param argc number of values
 * @param values pointer to the array with the values
 * @param index position where the values will be inserted
**/

void _arraylist_add_all_index(arraylist *list, size_t argc, void *values, size_t index){
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
    (list->values + (list->type * index))

/**
 * Delete an elements of the arraylist
 *
 * @param list arraylist to remove the value
 * @param index position of the value to remove
**/

void arraylist_remove_index(arraylist *list, size_t index){
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
 * Clear all the values from the list
 *
 * @param list: arraylist to clear
**/

void arraylist_clear(arraylist *list){
    // DELETEME

    // if(list->handler)
    //     for(size_t i = 0; i < list->size; i++)
    //         (*(list->handler))(*(void **)arraylist_get(list, i));

    if(list->handler)
        arraylist_foreach(list, item)
            (*(list->handler))(*(void **)item);

    list->size = 0;
}

/**
 * Free the list
 *
 * @param list: arraylist to free
**/

void arraylist_free(arraylist *list){
    if(list->handler)
        arraylist_clear(list);
    
    free(list->values);
    free(list);
}

#undef ARRAYLIST_CHUNK_SIZE

