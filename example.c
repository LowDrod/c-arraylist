#include <stdio.h>
#include "arraylist.c"

void print_arraylist_char(arraylist *list){
    printf("[ ");

    // NOTE: Item is a POINTER to the value
    arraylist_foreach(item, list){
        printf("\'%c\' ", *(char *)item);
    }

    printf("]\n");
}

int main(void){
    // ----- ----- ----- CREATE ----- ----- ----- //

    // Create list
    arraylist *list = arraylist_create(char);

    // ----- ----- ----- ADD ----- ----- ----- //

    // Add value
    arraylist_add(list, 'a');

    printf("\nAdding \'a\'\n");
    print_arraylist_char(list);

    // Add value by index
    arraylist_add_index(list, 'b', 0);

    printf("\nAdding \'b\' at index \"0\"\n");
    print_arraylist_char(list);

    // Add range of values
    char string[] = "cd";
    arraylist_add_all(list, sizeof(string) - 1, string);

    printf("\nAdding \"cd\"\n");
    print_arraylist_char(list);

    // Add range of values by index
    char other_string[] = "ef";
    arraylist_add_all_index(list, sizeof(string) - 1, other_string, 1);

    printf("\nAdding \"ef\" at index \"1\"\n");
    print_arraylist_char(list);

    // ----- ----- ----- REMOVE ----- ----- ----- //
    
    // Delete value
    arraylist_remove(list, 0);

    printf("\nRemoving at index \"1\"\n");
    print_arraylist_char(list);

    // Delete range values
    arraylist_remove_range(list, list->size - 3, list->size - 1);

    printf("\nRemoving last 3 values\n");
    print_arraylist_char(list);

    // ----- ----- ----- CLEAR ----- ----- ----- //

    // Clear
    arraylist_clear(list);

    printf("\nClear\n");
    print_arraylist_char(list);

    // ----- ----- ----- OPTIMIZE ----- ----- ----- //
    
    printf("\nOPTIMIZE\nBEFORE: Allocated space == %zu\n", list->capacity);
    arraylist_optimize(list);
    printf("AFTER:  Allocated space == %zu\n", list->capacity);
    
    // Free
    arraylist_free(list);
    
    // ----- ----- ----- HANDLER ----- ----- ----- //

    // NOTE: list->handler store the function necessary to free the arraylist values

    list = arraylist_create(arraylist *);

    // Remove handler
    arraylist_remove_handler(list);

    // Add handler
    arraylist_set_handler(list, arraylist_free);

    arraylist_add(list, arraylist_create(unsigned));

    // The handler automatically free the nested list
    arraylist_free(list);

    // ----- ----- ----- CLONE ----- ----- ----- //
    
    list = arraylist_create(float);
    
    for (float number_d = 11.11; number_d < 60; number_d += 11.11){
        arraylist_add(list, number_d);
    }

    arraylist *copy = arraylist_clone(list);

    printf("\nORIGINAL\n");
    arraylist_foreach(item, copy)
        printf("%5.2f  ", *(float *)item);

    printf("\n\nCOPY\n");
    arraylist_foreach(item, copy)
        printf("%5.2f  ", *(float *)item);

    printf("\n");

    arraylist_free(list);
    
    // ----- ----- ----- NESTING ----- ----- ----- //

    printf("\n\t NESTING\n");

    list = arraylist_create(arraylist *);
    arraylist_set_handler(list, arraylist_free);
    
    unsigned number = 0;

    for(unsigned i = 0; i < 22; i++){
        arraylist_add(list, arraylist_create(unsigned));

        arraylist *buffer = *(arraylist **)(arraylist_get(list, i));
        for(unsigned j = 0; j < 6; j++){
            arraylist_add(buffer, number++);
        }
    }
    
    printf("\n");

    arraylist_foreach(list_unsigned, list){
        arraylist_foreach(item, *(arraylist **)list_unsigned)
            printf("%4u", *(unsigned *)item);

        printf("\n");
    }

    printf("\n");

    arraylist_free(list);

    return 0;
}
