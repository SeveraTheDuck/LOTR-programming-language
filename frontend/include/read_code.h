#pragma once

#include <ctype.h>
#include "BinTree_struct.h"
#include "FileOpenLib.h"

struct token
{
    data_type token_data_type;
    union
    {
        op_code_type punct_op_code;
        op_code_type bin_op_code;
        op_code_type un_op_code;
        op_code_type key_op_code;
        double       num_value;
        char         var_name  [VAR_NAME_MAX_LEN];
    };
};

#include "List_struct.h"

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree);
