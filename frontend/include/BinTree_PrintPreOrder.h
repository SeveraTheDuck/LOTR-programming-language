#pragma once

#include "BinTree_struct.h"

const size_t PRINT_OUTPUT_ELEM_MAX_LEN = 10;

const size_t MAX_NODE_OUTPUT_LEN = 20;

const char TREE_OUTPUT_FILE_NAME[] = "../tree_out.txt";

void
PrintTreeToFile (const BinTree* const tree,
                 const char*    const out_file_name = TREE_OUTPUT_FILE_NAME);
