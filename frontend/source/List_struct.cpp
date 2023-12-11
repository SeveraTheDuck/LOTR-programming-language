#include "../include/List_struct.h"

static List_error_type
List_CtorAllocate   (List* const list);

static List_error_type
List_CtorFill       (List* const list);

static List_error_type
List_DtorPoison     (List* const list);

static List_error_type
List_DtorFree       (List* const list);

static List_error_type
List_CheckErrorOccurence (const List_errors* const     list_errors_field,
                          const List_errors* const ref_list_errors_field);

List_error_type
List_Ctor (List* const list)
{
    if (!list)
    {
        fprintf (stderr, "List_ERROR_LIST_NULLPTR\n");
        return List_ERROR_OCCURED;
    }

    List_error_type allocation_error = List_CtorAllocate (list);
    if (allocation_error)
    {
        return List_ERROR_OCCURED;
    }

    List_CtorFill      (list);

    List_VerifyAndDump (list);

    return List_NO_ERROR;
}

static List_error_type
List_CtorAllocate (List* const list)
{
    list->list_capacity = List_INIT_VOLUME;
    list->list_n_elems  = 1;

    list->list_data = (List_data_type*) calloc (List_INIT_VOLUME,
                                            sizeof (List_data_type));
    if (!list->list_data)
    {
        perror ("list->list_data allocation error");
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_DATA_NULLPTR = 1;

        return List_ERROR_OCCURED;
    }

    list->next = (uint32_t*) calloc (List_INIT_VOLUME,
                                     sizeof (uint32_t));
    if (!list->next)
    {
        perror ("list->next allocation error");
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_NEXT_NULLPTR = 1;

        return List_ERROR_OCCURED;
    }

    list->prev = (int32_t*) calloc (List_INIT_VOLUME,
                                    sizeof (int32_t));
    if (!list->prev)
    {
        perror ("list->prev allocation error");
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_PREV_NULLPTR = 1;

        return List_ERROR_OCCURED;
    }

    return List_NO_ERROR;
}

static List_error_type
List_CtorFill (List* const list)
{
    List_Assert (list);

    for (size_t list_index = 0;
                list_index < List_INIT_VOLUME;
              ++list_index)
    {
        list->next     [list_index] = (uint32_t) list_index + 1;
        list->prev     [list_index] = List_NO_PREV_ELEMENT;
    }

    list->next [ List_INIT_VOLUME - 1 ] = 0;
    list->next [List_DUMMY_ELEMENT] = 0;
    list->prev [List_DUMMY_ELEMENT] = 0;

    list->free = 1;

    memset (&list->list_errors_field,     0, sizeof(List_errors));
    memset (&list->ref_list_errors_field, 0, sizeof(List_errors));

    return List_NO_ERROR;
}

List_error_type
List_Dtor (List* const list)
{
    assert (list);

    List_DtorPoison (list);
    List_DtorFree   (list);

    return List_NO_ERROR;
}

static List_error_type
List_DtorPoison (List* const list)
{
    List_Assert (list);

    for (size_t list_index = 0;
                list_index < list->list_capacity;
              ++list_index)
    {
        list->next     [list_index] = 0;
        list->prev     [list_index] = List_NO_PREV_ELEMENT;
    }

    list->next [List_DUMMY_ELEMENT] = 0;
    list->prev [List_DUMMY_ELEMENT] = 0;
    list->free = 0;

    memset (&list->list_errors_field, 0, sizeof(List_errors));

    return List_NO_ERROR;
}

static List_error_type
List_DtorFree (List* const list)
{
    assert (list);

    free (list->list_data);
    free (list->next);
    free (list->prev);

    list->list_data = nullptr;
    list->next      = nullptr;
    list->prev      = nullptr;

    return List_NO_ERROR;
}

List_error_type
List_Verify (List* const list)
{
    if (!list)
    {
        return List_ERROR_OCCURED;
    }

    if (!list->list_data)
    {
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_DATA_NULLPTR = 1;
    }

    if (!list->next)
    {
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_NEXT_NULLPTR = 1;
    }

    if (!list->prev)
    {
        list->list_errors_field.errors_struct
            .List_ERROR_LIST_PREV_NULLPTR = 1;
    }

    if (list->next [List_DUMMY_ELEMENT] < 0 ||
        list->next [List_DUMMY_ELEMENT] >= (long long) list->list_capacity)
    {
        list->list_errors_field.errors_struct
            .List_ERROR_HEAD_OUT_OF_RANGE = 1;
    }

    if (list->prev [List_DUMMY_ELEMENT] < 0 ||
        list->prev [List_DUMMY_ELEMENT] >= (long long) list->list_capacity)
    {
        list->list_errors_field.errors_struct
            .List_ERROR_HEAD_OUT_OF_RANGE = 1;
    }

    if (list->free < 0 ||
        list->free >= (long long) list->list_capacity)
    {
        list->list_errors_field.errors_struct
            .List_ERROR_FREE_OUT_OF_RANGE = 1;
    }

    if (list->list_n_elems < 0 ||
        list->list_n_elems > list->list_capacity)
    {
        list->list_errors_field.errors_struct
            .List_ERROR_NUMBER_OF_ELEMENTS_OUT_OF_RANGE = 1;
    }

    return List_CheckErrorOccurence (&list->list_errors_field,
                                     &list->ref_list_errors_field);
}

static List_error_type
List_CheckErrorOccurence (const List_errors* const     list_errors_field,
                              const List_errors* const ref_list_errors_field)
{
    if (memcmp (list_errors_field,
            ref_list_errors_field,
            sizeof (List_errors)))
    {
        return List_ERROR_OCCURED;
    }

    return List_NO_ERROR;
}
