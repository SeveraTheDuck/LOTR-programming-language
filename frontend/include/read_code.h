#pragma once

#include <ctype.h>
#include "BinTree_struct.h"
#include "FileOpenLib.h"

union token_data
{
    double num_value;
    int8_t op_code;
    char   var_name [VAR_NAME_MAX_LEN];
};

struct token
{
    data_type  token_data_type;
    token_data data;
};

#include "List_struct.h"

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree);
