#pragma once

#include "BinTree_struct.h"

const size_t FIRST_FUNC_NUMBER = 1;

enum op_status
{
    NOT_IN_OPERATION = 0,
    IN_OPERATION     = 1
};

void
PrintTreeToAsm (const BinTree* const tree);
