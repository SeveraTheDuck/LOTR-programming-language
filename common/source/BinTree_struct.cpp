#include "BinTree_struct.h"

static BinTree_error_type
BinTree_CheckCycle (const BinTree_node* const node,
                          BinTree*      const tree,
                          uint32_t*     const counted_n_elements);

static BinTree_error_type
BinTree_CheckThreads (const BinTree_node* const node,
                            BinTree*      const tree);

BinTree_error_type
BinTree_Ctor       (BinTree* const tree,
                    BINTREE_CTOR_RECIVE_INFO)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return BINTREE_STRUCT_NULLPTR;
    }

    tree -> init_name = init_name;
    tree -> init_line = init_line;
    tree -> init_file = init_file;
    tree -> init_func = init_func;

    for (op_code_type key_word = 0;
                      key_word < NUM_OF_KEY_WORDS;
                      key_word++)
    {
        tree -> name_table .key_words_array  [key_word]
            .op_code = key_word;
        tree -> name_table .key_words_array  [key_word]
            .key_word_name = KEY_WORDS_ARRAY [key_word];
    }

    tree -> root        = nullptr;
    tree -> n_elem      = 0;
    tree -> errors      = 0;

    tree -> name_table .var_table  = (Stack*) calloc (1, sizeof (Stack));
    tree -> name_table .func_table = (Stack*) calloc (1, sizeof (Stack));

    STACK_CTOR (tree -> name_table .var_table);
    STACK_CTOR (tree -> name_table .func_table);

    return tree->errors;
}

BinTree_node*
BinTree_CtorNode   (const data_type data_type,
                    const double    data_value,
                    BinTree_node* const left,
                    BinTree_node* const right,
                    BinTree_node* const parent,
                    BinTree*      const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return nullptr;
    }

    BinTree_node* new_node = (BinTree_node*)
           calloc (1, sizeof (BinTree_node));
    if (!new_node)
    {
        perror ("new_node calloc error");
        tree->errors |= BINTREE_NODE_NULLPTR;

        return nullptr;
    }

    new_node -> data .data_type = data_type;

    switch (data_type)
    {
        case PUNCTUATION:
            new_node -> data .punct_op_code =
                (op_code_type) data_value;
            break;

        case BIN_OP:
            new_node -> data .bin_op_code =
                (op_code_type) data_value;
            break;

        case UN_OP:
            new_node -> data .un_op_code =
                (op_code_type) data_value;
            break;

        case KEY_OP:
            new_node -> data .key_op_code =
                (op_code_type) data_value;
            break;

        case NUMBER:
            new_node -> data .num_value = data_value;
            break;

        case VARIABLE:
            new_node -> data .var_index =
                (var_index_type) data_value;
            break;

        case FUNCTION:
            new_node -> data .func_index =
                (var_index_type) data_value;
            break;

        case NO_TYPE:
            [[fallthrough]];

        default:
            new_node -> data .data_type = NO_TYPE;
            fprintf (stderr, "Unknown type of node\n");

            free (new_node);
            return nullptr;
    }

    new_node -> left   = left;
    new_node -> right  = right;
    new_node -> parent = parent;

    tree -> n_elem++;

    return new_node;
}

// change name?
BinTree_node*
MakeNodeByData (BinTree_node*      const node_left,
                BinTree_data_type* const node_data,
                BinTree_node*      const node_right,
                BinTree_node*      const parent,
                BinTree*           const tree)
{
    assert (node_data);
    assert (tree);

    switch (node_data -> data_type)
    {
        case PUNCTUATION:
            return BinTree_CtorNode (PUNCTUATION, node_data -> punct_op_code,
                                     node_left, node_right, parent, tree);

        case BIN_OP:
            return BinTree_CtorNode (BIN_OP, node_data -> bin_op_code,
                                     node_left, node_right, parent, tree);

        case UN_OP:
            return BinTree_CtorNode (UN_OP, node_data -> un_op_code,
                                     node_left, node_right, parent, tree);

        case KEY_OP:
            return BinTree_CtorNode (UN_OP, node_data -> key_op_code,
                                     node_left, node_right, parent, tree);

        case NUMBER:
            return BinTree_CtorNode (NUMBER, node_data -> num_value,
                                     node_left, node_right, parent, tree);

        case VARIABLE:
            return BinTree_CtorNode (VARIABLE, node_data -> var_index,
                                     node_left, node_right, parent, tree);

        case FUNCTION:
            return BinTree_CtorNode (FUNCTION, node_data -> func_index,
                                     node_left, node_right, parent, tree);

        case NO_TYPE:
            [[fallthrough]];

        default:
            fprintf (stderr, "Wrong data type\n");
            return nullptr;
    }
}

BinTree_error_type
BinTree_DestroySubtree (BinTree_node* const node,
                        BinTree*      const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return BINTREE_STRUCT_NULLPTR;
    }

    if (!node)
    {
        return BINTREE_NODE_NULLPTR;
    }

    BinTree_DestroySubtree (node->left,  tree);
    BinTree_DestroySubtree (node->right, tree);

    node -> data .data_type = NO_TYPE;
    node -> data .num_value = BinTree_POISON;
    node -> left   = nullptr;
    node -> right  = nullptr;
    node -> parent = nullptr;

    tree->n_elem--;

    free (node);

    return NO_ERRORS;
}

BinTree_error_type
BinTree_DestroyNameTable (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return BINTREE_STRUCT_NULLPTR;
    }

    STACK_DTOR (tree -> name_table .var_table);
    STACK_DTOR (tree -> name_table .func_table);

    for (op_code_type key_word_index = 0;
                      key_word_index <= NUM_OF_KEY_WORDS;
                      key_word_index++)
    {
        tree -> name_table .key_words_array [key_word_index]
            .key_word_name = nullptr;
        tree -> name_table .key_words_array [key_word_index]
            .op_code = OP_CODE_POISON;
    }

    return NO_ERRORS;
}

BinTree_error_type
BinTree_Verify (BinTree* const tree)
{
    if (!tree)
    {
        return BINTREE_STRUCT_NULLPTR;
    }

    uint32_t counted_n_elements = 0;
    BinTree_CheckCycle (tree->root, tree, &counted_n_elements);

    if (counted_n_elements != tree->n_elem && tree->n_elem > 0)
    {
        tree->errors |= BINTREE_CYCLE_FOUND;
    }

    BinTree_CheckThreads (tree->root, tree);

    return tree->errors;
}

static BinTree_error_type
BinTree_CheckThreads (const BinTree_node* const node,
                            BinTree*      const tree)
{
    assert (tree);

    if (!tree->root)
    {
        return BINTREE_ROOT_NULLPTR;
    }

    if (node->data .data_type == NUMBER &&
       (node->left != nullptr || node->right != nullptr))
    {
        tree->errors |= LANGUAGE_NUMBER_WRONG_CHILDREN;
    }

    if (node->data .data_type == BIN_OP &&
       (node->left == nullptr || node->right == nullptr))
    {
        tree->errors |= LANGUAGE_OPERATION_WRONG_CHILDREN;
    }

    if (node->left)
        BinTree_CheckThreads (node->left,  tree);
    if (node->right)
        BinTree_CheckThreads (node->right, tree);

    return tree->errors;
}

static BinTree_error_type
BinTree_CheckCycle (const BinTree_node* const node,
                          BinTree*      const tree,
                          uint32_t*     const counted_n_elements)
{
    assert (tree);

    if (!node)
    {
        tree->errors |= BINTREE_NODE_NULLPTR;
        return tree->errors;
    }

    (*counted_n_elements)++;

    if (node->left)
        BinTree_CheckCycle (node->left,  tree, counted_n_elements);
    if (node->right)
        BinTree_CheckCycle (node->right, tree, counted_n_elements);

    if (*counted_n_elements != tree->n_elem)
    {
        return BINTREE_CYCLE_FOUND;
    }

    return NO_ERRORS;
}

BinTree_node*
CopyNode (BinTree_node* const node,
          BinTree_node* const parent,
          BinTree*      const c_tree)
{
    if (!node) return nullptr;

    return MakeNodeByData (CopyNode (node->left, node->parent, c_tree),
                           &node->data,
                           CopyNode (node->right, node->parent, c_tree),
                           parent, c_tree);
}

void
SetParents (BinTree_node* const parent,
            BinTree_node* const node)
{
    assert (node);

    node->parent = parent;

    if (node->left)
    {
        SetParents (node, node->left);
    }

    if (node->right)
    {
        SetParents (node, node->right);
    }
}
