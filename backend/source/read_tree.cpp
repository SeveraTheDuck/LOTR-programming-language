#include "read_tree.h"

#define SKIP_SPACES(input, input_shift)     \
    while (isspace (input [*input_shift]))  \
    {                                       \
        ++(*input_shift);                   \
    }

static char*
OpenInputFile (const char* const input_file_name);

static BinTree_node*
ReadNode (BinTree*      const tree,
          BinTree_node* const parent,
          const char*   const input,
          size_t*       const input_shift);

BinTree*
ReadTreeFromFile (      BinTree* const tree,
                  const char*    const input_file_name)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer.\n");
        return nullptr;
    }

    char* const input_str = OpenInputFile (input_file_name);
    if (!input_str) return nullptr;

    size_t input_shift = 0;

    tree -> root = ReadNode (tree, nullptr, input_str, &input_shift);

    free (input_str);

    return tree;
}

static BinTree_node*
ReadNode (BinTree*      const tree,
          BinTree_node* const parent,
          const char*   const input_str,
          size_t*       const input_shift)
{
    assert (tree);
    assert (input_str);
    assert (input_shift);

    SKIP_SPACES (input_str, input_shift);

    if (input_str [*input_shift] == '(')
    {
        (*input_shift)++;
        SKIP_SPACES (input_str, input_shift);
    }

    else if (input_str [*input_shift] == '_')
    {
        (*input_shift)++;
        SKIP_SPACES (input_str, input_shift);

        return nullptr;
    }

    data_type data_type  = NO_TYPE;
    double    data_value = BinTree_POISON;

    uint32_t n_read_symbols = 0;

    sscanf (input_str + *input_shift, "%d %lg %n",
            &data_type, &data_value, &n_read_symbols);

    (*input_shift) += n_read_symbols;

    SKIP_SPACES (input_str, input_shift);

    BinTree_node* const new_node =
        BinTree_CtorNode (data_type, data_value, nullptr,
                          nullptr, parent, tree);

    new_node->left  = ReadNode (tree, new_node, input_str, input_shift);
    new_node->right = ReadNode (tree, new_node, input_str, input_shift);

    SKIP_SPACES (input_str, input_shift);

    if (input_str [*input_shift] !=  ')')
    {
        fprintf (stderr, "Wrong file input! No closing bracket\n");
        return nullptr;
    }

    else
    {
        (*input_shift)++;
        return new_node;
    }
}

static char*
OpenInputFile (const char* const input_file_name)
{
    assert (input_file_name);

    FILE* input_file = fopen (input_file_name, "rb");
    if (!input_file)
    {
        perror ("Unable to open input file with tree");
        return nullptr;
    }

    fseek  (input_file, 0, SEEK_END);
    size_t input_len = (size_t) ftell(input_file);
    fseek  (input_file, 0, SEEK_SET);

    char* const input_tree = (char*) calloc (input_len + 1, sizeof (char));
    if (!input_tree)
    {
        perror ("input_tree allocation error");
        return nullptr;
    }

    fread (input_tree, sizeof (char), input_len, input_file);

    fclose (input_file);

    return input_tree;
}
