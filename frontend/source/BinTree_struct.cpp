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

    tree->init_name = init_name;
    tree->init_line = init_line;
    tree->init_file = init_file;
    tree->init_func = init_func;

    tree->name_table.var_table =
        (variable*) calloc (INIT_VAR_NUMBER, sizeof (variable));
    if (!tree->name_table.var_table)
    {
        perror ("var_table allocation error");
        tree->errors |= BINTREE_VAR_TABLE_NULLPTR;
    }

    for (op_code_type key_word = 0;
                      key_word < NUM_OF_KEY_WORDS;
                      key_word++)
    {
        tree->name_table.key_words_array [key_word]
            .op_code = key_word;
        tree->name_table.key_words_array [key_word]
            .key_word_name = KEY_WORDS_ARRAY [key_word];
    }

    tree->root       = nullptr;
    tree->n_elem     = 0;
    tree->errors     = 0;
    tree->var_number = 0;
    tree->var_table_capacity = INIT_VAR_NUMBER;

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

    new_node->data .data_type = data_type;

    if (data_type == NUMBER)
    {
        new_node->data .data_value .num_value = data_value;
    }

    else if (data_type == OPERATION)
    {
        new_node->data .data_value .op_code =
            (op_code_type) data_value;
    }

    else if (data_type == VARIABLE)
    {
        new_node->data .data_value .var_index =
            (var_index_type) data_value;
    }

    else
    {
        new_node->data .data_type = NO_TYPE;
        fprintf (stderr, "Unknown type of node\n");

        free (new_node);
        return nullptr;
    }

    new_node->left   = left;
    new_node->right  = right;
    new_node->parent = parent;

    tree->n_elem++;

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

    if (node_data->data_type == NUMBER)
    {
        return BinTree_CtorNode (NUMBER, node_data->data_value .num_value,
                                 node_left, node_right, parent, tree);
    }

    else if (node_data->data_type == OPERATION)
    {
        return BinTree_CtorNode (OPERATION, node_data->data_value .op_code,
                                 node_left, node_right, parent, tree);
    }

    else if (node_data->data_type == VARIABLE)
    {
        return BinTree_CtorNode (VARIABLE, node_data->data_value .var_index,
                                 node_left, node_right, parent, tree);
    }

    else
    {
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

    node->data .data_type = NO_TYPE;
    node->data .data_value .num_value = BinTree_POISON;
    node->left   = nullptr;
    node->right  = nullptr;
    node->parent = nullptr;

    tree->n_elem--;

    free (node);

    return NO_ERRORS;
}

BinTree_error_type
BinTree_DestroyVarTable (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return BINTREE_STRUCT_NULLPTR;
    }

    for (var_index_type var_index = 0;
                        var_index < tree->var_number;
                        var_index++)
    {
        tree->name_table.var_table [var_index] .var_value = BinTree_POISON;
        free (tree->name_table.var_table[var_index] .var_name);
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

    if (node->data .data_type == OPERATION &&
       (node->left == nullptr || node->right == nullptr))
    {
        switch (node->data .data_value .op_code)
        {
            /*
             * No breaks used in ADD, SUB and MUL because
             * the behavior is the same for all of them,
             * so switch can move through them.
             * There are few of them, so it is not slow.
             */
            case ASSUME_BEGIN:
                [[fallthrough]];
            case ADD:
                [[fallthrough]];
            case SUB:
                [[fallthrough]];
            case MUL:
                [[fallthrough]];
            case DIV:
                tree->errors |= LANGUAGE_OPERATION_WRONG_CHILDREN;
                break;
        }
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

variable*
ReallocVarTable (BinTree* const tree)
{
    assert (tree);

    tree->name_table.var_table =
        (variable*) realloc (tree->name_table.var_table,
                             (size_t) tree->var_table_capacity *
                             sizeof (variable));

    return tree->name_table.var_table;
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
