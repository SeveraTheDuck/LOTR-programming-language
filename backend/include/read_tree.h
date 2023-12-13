#pragma once

#include <ctype.h>
#include "BinTree_struct.h"

BinTree*
ReadTreeFromFile (      BinTree* const tree,
                  const char*    const input_file_name);
