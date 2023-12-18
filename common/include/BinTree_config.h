#pragma once

#include "language_config.h"

typedef data BinTree_data_type;

#define BinTree_OUTPUT_F "%lg"

const double BinTree_POISON = -3.1415926;

const char* const BinTree_IMAGE_CONSTRUCT_FILE_NAME = "image_construct.dot";
const char* const BinTree_IMAGE_FILE_NAME = "tree.png";

const size_t VAR_NAME_MAX_LEN = 50;
const size_t INIT_VAR_NUMBER  = 10;

const size_t FUNC_NAME_MAX_LEN = 50;
const size_t INIT_FUNC_NUMBER  = 10;

const var_index_type VAR_TABLE_CAPACITY_MULTIPLIER = 2;
