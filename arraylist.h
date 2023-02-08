#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ARRAYLIST_INITIAL_CAPACITY 16
#define ARRAYLIST_GROWTH_FACTOR 2

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

#define _arraylist_foreach(item, list, i)                 \
    for(                                                  \
        uintptr_t item = (uintptr_t) list->values, i = 0; \
        i++ < list->size;                                 \
        item += list->type                                \
    )

// Reverse foreach

#define arraylist_reverse_foreach(item, list) \
    _arraylist_reverse_foreach(item, (list), _iterator_##item)

#define _arraylist_reverse_foreach(item, list, i)                                          \
    for(                                                                                   \
        uintptr_t item = ((uintptr_t)list->values) + (list->size - 1) * list->type, i = 0; \
        i++ < list->size;                                                                  \
        item -= list->type                                                                 \
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

    list->values = malloc(type * ARRAYLIST_INITIAL_CAPACITY);
    list->type = type;
    list->size = 0;
    list->capacity = ARRAYLIST_INITIAL_CAPACITY;
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

    size_t initial_capacity = ARRAYLIST_INITIAL_CAPACITY;

    if (size > initial_capacity){
        size_t tmp = size / ARRAYLIST_INITIAL_CAPACITY;
        initial_capacity *= ARRAYLIST_GROWTH_FACTOR;

        while (tmp > ARRAYLIST_GROWTH_FACTOR) {
            tmp /= ARRAYLIST_GROWTH_FACTOR;
            initial_capacity *= ARRAYLIST_GROWTH_FACTOR;
        }
    }

    list->values = malloc(type * initial_capacity);
    list->type = type;
    list->size = 0;
    list->capacity = initial_capacity;
    list->handler = NULL;

    return list;
}

/**
 * Reallocate space for the new value
 *
 * @param list arraylist to resize
 * @param index position where the value will be inserted
 *
 * @return address where the value will be inserted
**/

void *arraylist_allocate(arraylist *list, size_t index){
    if(list->size++ >= list->capacity){
        list->capacity *= ARRAYLIST_GROWTH_FACTOR;
        list->values = realloc(list->values, list->capacity * list->type);
    }
    
    char *zone_to_move = ((char *)list->values) + index * list->type;

    memmove(
        zone_to_move + list->type,
        zone_to_move,
        list->type * (list->size - index - 1)
    );

    return zone_to_move;
}

/**
 * Reallocate space for the new values
 *
 * @param list arraylist to resize
 * @param index position where the values will be inserted
 * @param size size to resize
 *
 * @return address where the values will be inserted
**/

void *arraylist_allocate_all(arraylist *list, size_t index, size_t size){
    list->size += size;

    if(list->size > list->capacity){
        do {
            list->capacity *= ARRAYLIST_GROWTH_FACTOR;
        } while (list->capacity < list->size);

        list->values = realloc(list->values, list->capacity * list->type);
    }

    char *zone_to_move = ((char *)list->values) + index * list->type;

    memmove(
        zone_to_move + size * list->type,
        zone_to_move,
        list->type * (list->size - index - size)
    );

    return zone_to_move;
}

/*
 * Resize the arraylist to fit with the values size
 *
 * @param list arraylist to resize
**/

void arraylist_optimize(arraylist *list){
    if (list->size == 0) {
        list->capacity = ARRAYLIST_INITIAL_CAPACITY;
        list->values = realloc(list->values, list->type * ARRAYLIST_INITIAL_CAPACITY);
        return;
    }

    if (list->capacity / ARRAYLIST_GROWTH_FACTOR < list->size)
        return;
    
    while (list->capacity >= list->size)
        list->capacity /= ARRAYLIST_GROWTH_FACTOR;

    list->capacity *= ARRAYLIST_GROWTH_FACTOR;
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

#define arraylist_add_index(list, value, index) \
    memcpy(arraylist_allocate(list, index), value, (list)->type);

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

#define arraylist_add_all_index(list, argc, values, index) \
    memcpy(arraylist_allocate_all(list, index, argc), values, list->type * argc)

/**
 * Return a pointer to a value based in an index
 *
 * @param list arraylist to get the value
 * @param index position of the value
 *
 * @return a pointer to the value
**/

#define arraylist_get(list, index) \
    (((uintptr_t)(list)->values) + ((list)->type * (index)))

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
    char *zone_to_move = ((char *)list->values) + index * list->type;
    
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
        ((char *)list->values) + start * list->type,
        ((char *)list->values) + end * list->type,
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

arraylist *arraylist_slice(arraylist *src, size_t index, size_t size){
    arraylist *dest = (arraylist *) malloc(sizeof(arraylist));

    dest->type = src->type;
    dest->size = size;
    dest->handler = src->handler;

    dest->capacity = ARRAYLIST_INITIAL_CAPACITY;
    while (dest->capacity < size)
        dest->capacity *= ARRAYLIST_GROWTH_FACTOR;

    dest->values = malloc(dest->type * dest->capacity);

    memcpy(
        dest->values,
        ((char *)src->values) + index * src->type,
        dest->type * size
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
#endif // ARRAYLIST_H
