#pragma once

#include "List_struct.h"

uint32_t
List_PushBack  (List_data_type* const value,
                List*           const list);

uint32_t
List_PushFront (List_data_type* const value,
                List*           const list);

/// @brief This function inserts value after given position.
/// @param position
/// @param value
/// @param list
/// @return Index of inserted element.
uint32_t
List_Insert (const uint32_t              position,
                   List_data_type* const value,
                   List*           const list);

/// @brief This function erases element on given position.
/// @note Invalids address of the last element (physical index in array) for linearization.
/// @param position
/// @param list
/// @return
// uint32_t
// List_Erase (const uint32_t        position,
//                       List* const list);

List_error_type
List_ReallocUp (List* const list);

List_error_type
List_ReallocDown (const size_t    new_capacity,
                      List* const list);
