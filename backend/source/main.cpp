#include "BinTree_make_image.h"
#include "read_tree.h"
#include "print_asm.h"

int main (const int32_t /*argc*/, const char** argv)
{
    BinTree tree = {};
    BINTREE_CTOR (&tree);

    ReadTreeFromFile (&tree, argv [1]);
    BinTree_MakeTreeImage (&tree);

    PrintTreeToAsm (&tree);

    BINTREE_DTOR (&tree);

    return 0;
}
