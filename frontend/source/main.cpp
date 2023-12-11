#include "read_code.h"
#include "BinTree_make_image.h"

int main (const int32_t /*argc*/, const char** argv)
{
    BinTree tree = {};
    BINTREE_CTOR (&tree);

    ReadTree (argv[1], &tree);
    BinTree_MakeTreeImage (&tree);

    BINTREE_DTOR (&tree);

    return 0;
}
