#pragma once

enum data_type
{
    NO_TYPE     = -1,
    PUNCTUATION =  0,
    BIN_OP      =  1,
    UN_OP       =  2,
    KEY_OP      =  3,
    NUMBER      =  4,
    VARIABLE    =  5,
};

enum un_op_code
{
    SIN = 0,
    COS,
    SQRT,
    LN,

    NOT,
    OUT,
    OUT_S,
    IN,

    CALL,
    RET,

    NUM_OF_UN_OP
};

enum bin_op_code
{
    ADD = NUM_OF_UN_OP,
    SUB,
    MUL,
    DIV,
    POW,

    IS_EQUAL,
    GREATER,
    LESS,
    GREATER_OR_EQUAL,
    LESS_OR_EQUAL,
    NOT_QEUAL,

    ASSUME_BEGIN,

    NUM_OF_BIN_OP
};

enum key_op_code
{
    IF = NUM_OF_BIN_OP,
    WHILE,

    NUM_OF_KEY_OP
};

enum punctuation
{
    END_OF_OPERATION = NUM_OF_KEY_OP,

    NUM_OF_KEY_WORDS
};

typedef int8_t op_code_type;
typedef int8_t var_index_type;

const op_code_type OP_CODE_POISON = -1;

struct data
{
    data_type data_type;
    union
    {
        op_code_type   punct_op_code;
        op_code_type   bin_op_code;
        op_code_type   un_op_code;
        op_code_type   key_op_code;
        double         num_value;
        var_index_type var_index;
    };
};
