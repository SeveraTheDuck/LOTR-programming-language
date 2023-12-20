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
    FUNCTION    =  6,
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

    RET,

    NUM_OF_UN_OP // 9
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

    NUM_OF_BIN_OP // 22
};

enum key_op_code
{
    IF = NUM_OF_BIN_OP,
    WHILE,

    NUM_OF_KEY_OP // 24
};

enum punctuation
{
    END_OF_OPERATION = NUM_OF_KEY_OP,

    NULL_TERMINATOR,

    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,

    OPEN_BRACE,
    CLOSE_BRACE,

    ASSUME_END,

    FUNC_DEF,
    FUNC_ARGS_BEGIN,
    FUNC_ARGS_END,
    COMMA,

    NUM_OF_KEY_WORDS // 35
};

typedef int8_t op_code_type;
typedef size_t var_index_type;

const op_code_type OP_CODE_POISON = -1;
const var_index_type VAR_INDEX_POISON = UINT64_MAX;

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
        var_index_type func_index;
    };
};

const char* const KEY_WORDS_ARRAY [NUM_OF_KEY_WORDS] =
    {
     "SIN", "COS", "SQRT", "LN", "!", "OUT", "OUT_S", "IN",
     "Return of the King",                                  // ret

     "ADD", "SUB", "MUL", "DIV", "POW",
     "==", ">", "<", ">=", "<=", "!=",
     "Give him",                                            // =

     "One does not simply walk into Mordor",                // if
     "So it begins",                                        // while

     "Precious",                                            // ;
     "\0", "Unexpected", "Journey", "Black", "Gates",       // '\0' ( ) { }
     "A pony",                                              // = (second part)
     "Mellon",                                              // func_def
     "Fellowship", "of the Ring",                           // func_args
     "Gollum"                                               // ,
    };
