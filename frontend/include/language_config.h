#pragma once

enum data_type
{
    NO_TYPE = 0,
    NUMBER,
    OPERATION,
    VARIABLE
};

enum op_code
{
    NULL_TERMINATOR   = 0x0,

    OPEN_PARENTHESIS  = 0x1,
    CLOSE_PARENTHESIS = 0x2,

    OPEN_BRACES       = 0x3,
    CLOSE_BRASES      = 0x4,

    END_OF_OPERATION  = 0x5,

    ASSUME_BEGIN = 0x6,
    ASSUME_END   = 0x7,
    IF           = 0x8,
    WHILE        = 0x9,

    ADD    = 0xA,
    SUB    = 0xB,
    MUL    = 0xC,
    DIV    = 0xD,

    NUM_OF_KEY_WORDS
};

typedef int8_t op_code_type;
typedef int8_t var_index_type;

union data_value
{
    double         num_value;
    op_code_type   op_code;
    var_index_type var_index;
};

struct data
{
    data_type  data_type;
    data_value data_value;
};

static const char* const KEY_WORDS_ARRAY [NUM_OF_KEY_WORDS] =
    {"\0",
     "Unexpected", "Journey", "Black", "Gates", "Precious", // ( ) { } ;
     "Give him", "A pony",                                  // =
     "One does not simply walk into Mordor",                // if
     "So it begins",                                        // while
     "ADD", "SUB", "MUL", "DIV"};                           // + - * /
