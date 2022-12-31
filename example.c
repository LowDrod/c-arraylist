#include <stdio.h>
#include "arraylist.c"

void print_arraylist_char(arraylist *list){
    printf("[ ");

    // NOTE: Item is a POINTER to the value
    arraylist_foreach(item, list){
        printf("\"%c\" ", *(char *)item);
    }

    printf("]\n");
}

int main(void){
    // ----- ----- ----- CREATE ----- ----- ----- //

    // Create list
    arraylist *list = arraylist_create(char);

    // ----- ----- ----- ADD ----- ----- ----- //

    // NOTE: You CAN'T store a value directly
    // arraylist_add(list, 'a');

    // Add value
    char character = 'a';
    arraylist_add(list, character);

    printf("\nAdding \'a\'\n");
    print_arraylist_char(list);

    // Add value by index
    character++;
    arraylist_add_index(list, character, 0);

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

    printf("\nRemoving from the last -2 to the last\n");
    print_arraylist_char(list);

    // ----- ----- ----- CLEAR ----- ----- ----- //

    // Clear
    arraylist_clear(list);

    printf("\nClear\n");
    print_arraylist_char(list);

    // ----- ----- ----- OPTIMIZE ----- ----- ----- //
    
    printf("\nOptimize\nBEFORE: Allocated space == %zu\n", list->capacity);
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

    arraylist *nested_list = arraylist_create(unsigned);
    arraylist_add(list, nested_list);

    arraylist_free(list); // The handler automatically free the nested list

    return 0;
}
