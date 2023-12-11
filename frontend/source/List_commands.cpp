#include "../include/List_commands.h"

static List_error_type
List_Realloc (List* const list);

uint32_t
List_PushBack (List_data_type* const value,
               List*    const list)
{
    List_VerifyAndDump (list);

    List_Insert ((uint32_t) list->prev [ List_DUMMY_ELEMENT ],
                     value, list);

    return (uint32_t) list->prev [ List_DUMMY_ELEMENT ];
}

uint32_t
List_PushFront (List_data_type* const value,
                          List*    const list)
{
    List_VerifyAndDump (list);

    List_Insert (List_DUMMY_ELEMENT, value, list);

    return list->next [ List_DUMMY_ELEMENT ];
}

uint32_t
List_Insert (const uint32_t       position,
             List_data_type* const value,
                   List*    const list)
{
    List_VerifyAndDump (list);

    if (position >= list->list_capacity ||
        list->prev [position] == List_NO_PREV_ELEMENT)
    {
        fprintf (stderr, "In function List_Insert() "
                         "position of element out of range.\n");
        return List_DUMMY_ELEMENT;
    }

    if (list->list_n_elems == list->list_capacity)
    {
        list->list_capacity *= List_EXPAND_MULTIPLIER;
        List_ReallocUp (list);
    }
    list->list_n_elems++;

    // insert itself
    uint32_t new_index = list->free;
    list->free = (uint32_t) list->next[ list->free ];

    list->prev [ list->next [position] ] = (int32_t) new_index;
    list->next [ new_index ] = list->next [position];

    list->next [ position ]  = new_index;
    list->prev [ new_index ] = (int32_t) position;

    memcpy (list->list_data + new_index, value, sizeof (List_data_type));

    return new_index;
}

/* don't need to use */
// uint32_t
// List_Erase (const uint32_t        position,
//                       List* const list)
// {
//     if (list->prev [position] == List_NO_PREV_ELEMENT)
//     {
//         fprintf (stderr, "In function List_Erase() "
//                          "position of element out of range.\n");
//         return List_DUMMY_ELEMENT;
//     }
//
//     else if (position == List_DUMMY_ELEMENT)
//     {
//         fprintf (stderr, "In function List_Erase() "
//                          "calling for dummy element, return.\n");
//         return List_DUMMY_ELEMENT;
//     }
//
//     list->list_n_elems--;
//
//     // connect next and prev of position
//     list->next [ list->prev [position] ] = list->next [position];
//     list->prev [ list->next [position] ] = list->prev [position];
//
//     uint32_t prev_index = (uint32_t) list->prev [position];
//
//     // move the last element for linearization
//     list->next      [position] = list->next [ list->list_n_elems ];
//     list->prev      [position] = list->prev [ list->list_n_elems ];
//     list->list_data [position] = list->list_data [ list->list_n_elems ];
//
//     list->next      [ list->list_n_elems ] = list->free;
//     list->prev      [ list->list_n_elems ] = List_NO_PREV_ELEMENT;
//     list->list_data [ list->list_n_elems ] = List_POISON;
//
//     list->free = (uint32_t) list->list_n_elems;
//
//     return prev_index;
// }

List_error_type
List_ReallocUp (List* const list)
{
    if (List_Realloc (list))
    {
        return List_ERROR_OCCURED;
    }

    for (size_t list_index = list->list_capacity / List_EXPAND_MULTIPLIER;
                list_index < list->list_capacity;
              ++list_index)
    {
        list->next     [list_index] = (uint32_t) list_index + 1;
        list->prev     [list_index] = List_NO_PREV_ELEMENT;
    }

    list->next [ list->list_capacity - 1] = 0;
    list->free = (uint32_t) (list->list_capacity / List_EXPAND_MULTIPLIER);

    return List_NO_ERROR;
}

List_error_type
List_ReallocDown (const size_t    new_capacity,
                      List* const list)
{
    size_t old_capacity = list->list_capacity;
    list->list_capacity = new_capacity;

    if (List_Realloc (list))
    {
        fprintf (stderr, "List_ReallocDown() error,"
                         "old capacity saved\n");
        list->list_capacity = old_capacity;

        return List_ERROR_OCCURED;
    }

    if (list->prev [ new_capacity - 1] == List_NO_PREV_ELEMENT)
    {
        list->next [ new_capacity - 1] = List_DUMMY_ELEMENT;
    }

    return List_NO_ERROR;
}

static List_error_type
List_Realloc (List* const list)
{
    list->list_data = (List_data_type*) realloc (list->list_data,
                                                     list->list_capacity *
                                                     sizeof (List_data_type));
    if (!list->list_data)
    {
        perror ("list->list_data reallocation error");
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_DATA_NULLPTR = 1;

        return List_ERROR_OCCURED;
    }

    list->next = (uint32_t*) realloc (list->next,
                             list->list_capacity * sizeof (uint32_t));
    if (!list->next)
    {
        perror ("list->next reallocation error");
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_NEXT_NULLPTR = 1;

        return List_ERROR_OCCURED;
    }

    list->prev = (int32_t*)  realloc (list->prev,
                             list->list_capacity * sizeof (uint32_t));
    if (!list->prev)
    {
        perror ("list->prev reallocation error");
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_PREV_NULLPTR = 1;

        return List_ERROR_OCCURED;
    }

    return List_NO_ERROR;
}
