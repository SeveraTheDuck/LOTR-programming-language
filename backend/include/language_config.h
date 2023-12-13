#pragma once

enum data_type
{
    NO_TYPE     = -1,
    PUNCTUATION =  0,
    BIN_OP      =  1,
    UN_OP       =  2,
    NUMBER      =  3,
    VARIABLE    =  4,
};

enum punctuation
{
    NULL_TERMINATOR   = 0,

    OPEN_PARENTHESIS  = 1,
    CLOSE_PARENTHESIS = 2,

    OPEN_BRACE        = 3,
    CLOSE_BRACE       = 4,

    END_OF_OPERATION  = 5,

    NUM_OF_PUNCT_SYMBOLS
};

enum bin_op_code
{
    ADD              = 0,
    SUB              = 1,
    MUL              = 2,
    DIV              = 3,
    POW              = 4,

    IS_EQUAL         = 5,
    GREATER          = 6,
    LESS             = 7,
    GREATER_OR_EQUAL = 8,
    LESS_OR_EQUAL    = 9,
    NOT_QEUAL        = 10,

    ASSUME_BEGIN     = 11,
    ASSUME_END       = 12,

    NUM_OF_BIN_OP
};

enum un_op_code
{
    SIN   = 0,
    COS   = 1,
    SQRT  = 2,
    LN    = 3,

    NOT   = 4,
    OUT   = 5,
    OUT_S = 6,
    IN    = 7,

    IF    = 8,
    WHILE = 9,

    NUM_OF_UN_OP
};

typedef int8_t op_code_type;
typedef int8_t var_index_type;

const op_code_type NUM_OF_KEY_WORDS = NUM_OF_PUNCT_SYMBOLS +
                                      NUM_OF_BIN_OP + NUM_OF_UN_OP;

const op_code_type OP_CODE_POISON = -1;

struct data
{
    data_type data_type;
    union
    {
        op_code_type   bin_op_code;
        op_code_type   un_op_code;
        double         num_value;
        var_index_type var_index;
        op_code_type   punct_op_code;
    };
};
