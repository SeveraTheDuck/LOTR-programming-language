#include "read_code.h"
#include "BinTree_make_image.h"
#include "BinTree_PrintPreOrder.h"

int main (const int32_t argc, const char** argv)
{
    BinTree tree = {};
    BINTREE_CTOR (&tree);

    ReadTree (argv[1], &tree);
    BinTree_MakeTreeImage (&tree);

    if (argc < 3)
    {
        PrintTreeToFile (&tree);
    }

    else
    {
        PrintTreeToFile (&tree, argv [2]);
    }

    BINTREE_DTOR (&tree);

    return 0;
}
