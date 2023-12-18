#pragma once

/// @brief This typedef defines type for elements of stack. Change it to the type you like.
typedef char* Elem_t;

/// @brief This const is used for poison value (empty space in stack).
/// Change it if you have changed Elem_t type or you want to see different value.
const Elem_t POISON = nullptr;

/// @brief This define is for output specificator of Elem_t.
#define OUTPUT_F "%s"

/// @brief This define is for debug regime.
/// It provides more complex output for STACK_DUMP() and more information in stack struct.
#define _DEBUG

/// @brief This define enables canary protection.
/// Undefine or comment the string below to switch 'em off.
/// Canaries are located on both sides of stack struct and stack data.
#define CANARY_PROTECTION

/// @brief This define enables hash protection.
/// Undefine or comment the string below to switch it off.
/// The program uses simple hash protection - summ of bytes.
#define HASH_PROTECTION

/// @brief This const tells the program the initial capacity of stack.
const size_t INIT_CAPACITY      = 4;

/// @brief This const tells the program the multipier which by stack expands.
const size_t RESIZE_MULTIPLIER  = 2;
