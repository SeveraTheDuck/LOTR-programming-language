#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "List_config.h"

#define List_Assert(list)   \
    assert (list);              \
    assert (list->list_data);   \
    assert (list->next);        \
    assert (list->prev);

#define List_VerifyAndDump(list)    \
    if (List_Verify (list))         \
    {                               \
        List_Dtor (list);           \
        exit (List_ERROR_OCCURED);  \
    }

typedef unsigned short List_error_type;

const List_error_type List_ERROR_OCCURED = 1;
const List_error_type List_NO_ERROR      = 0;

const int8_t List_NO_PREV_ELEMENT = -1;

const int8_t List_DUMMY_ELEMENT = 0;

struct List_errors_struct
{
    List_error_type List_ERROR_LIST_NULLPTR                     : 1;
    List_error_type List_ERROR_LIST_DATA_NULLPTR                : 1;
    List_error_type List_ERROR_LIST_NEXT_NULLPTR                : 1;
    List_error_type List_ERROR_LIST_PREV_NULLPTR                : 1;

    List_error_type List_ERROR_HEAD_OUT_OF_RANGE                : 1;
    List_error_type List_ERROR_TAIL_OUT_OF_RANGE                : 1;
    List_error_type List_ERROR_FREE_OUT_OF_RANGE                : 1;
    List_error_type List_ERROR_NUMBER_OF_ELEMENTS_OUT_OF_RANGE  : 1;

    List_error_type List_ERROR_CYCLE_FOUND                      : 1;
};

/// @brief This union is used to avoid UB when compare two bit fields.
union List_errors
{
    List_errors_struct errors_struct;
    char filler_array[sizeof (List_errors_struct)];
};

struct List
{
    List_data_type* list_data;
    uint32_t* next;
    int32_t * prev;

    uint32_t free;

    size_t list_capacity;
    size_t list_n_elems;

    // Two bit fields, the first one is for errors.
    List_errors     list_errors_field;
    // The second one is for comparison, it is full of zeros.
    List_errors ref_list_errors_field;
};

List_error_type
List_Ctor           (List* const list);

List_error_type
List_Dtor           (List* const list);

List_error_type
List_Verify         (List* const list);
