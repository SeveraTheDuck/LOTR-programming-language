#ifndef CONST_H
#define CONST_H

/// @brief This is the main struct with stack. It is declared in stack.h
/// It contains stack data, its size and capacity, its initial info,
/// struct with errors, canaries and hash.
struct Stack;

/// @brief This typedef is used to use, mark and return errors.
typedef unsigned short Error_t;

#ifdef HASH_PROTECTION
    /// @brief This typedef is used to use, mark and return hash value.
    typedef unsigned long long Hash_t;
#endif

#ifdef CANARY_PROTECTION
    /// @brief This typedef is used to use, mark and return canary values.
    typedef unsigned long long Canary_t;

    /// @brief This define is for output specificator of Canary_t.
    #define CANARY_F "%llu"

    /// @brief This is const value for canaries. If you try to change them,
    /// @brief it will return errors.
    const Canary_t CANARY_VALUE = 31415926UL;
#endif

/// @brief This bit field is used to flag errors.
struct ErrorType
{
    Error_t STACK_ERROR_OCCURED                 : 1; ///< If errors have already occured. Returns to user.
    Error_t STACK_NULL_POINTER                  : 1; ///< If stack is nullptr.
    Error_t STACK_DATA_NULL_POINTER             : 1; ///< If stack data is nullptr.
    Error_t STACK_SIZE_NOT_LEGIT_VALUE          : 1; ///< If stack size is less than 0.
    Error_t STACK_CAPACITY_NOT_LEGIT_VALUE      : 1; ///< If stack capacity is less than 0.
    Error_t STACK_SIZE_OUT_OF_RANGE             : 1; ///< If stack size is greater than stack capacity.

    #ifdef CANARY_PROTECTION
        Error_t STACK_LEFT_CANARY_DAMAGED       : 1; ///< If left canary near stack struct is equal to CANARY_VALUE.
        Error_t STACK_RIGHT_CANARY_DAMAGED      : 1; ///< If right canary near stack struct is equal to CANARY_VALUE.
        Error_t STACK_DATA_LEFT_CANARY_DAMAGED  : 1; ///< If left canary near stack data is equal to CANARY_VALUE.
        Error_t STACK_DATA_RIGHT_CANARY_DAMAGED : 1; ///< If right canary near stack data is equal to CANARY_VALUE.
    #endif

    #ifdef HASH_PROTECTION
        Error_t STACK_HASH_DAMAGED              : 1; ///< If stack hash is not equal to new hash value.
    #endif
};

#endif
