#include "BinTree_PrintPreOrder.h"

static void
PrintInPreOrder (const BinTree_node* const node,
                       char*         const output_buf,
                       int32_t*       const output_index);

void
PrintTreeToFile (const BinTree* const tree,
                 const char*    const out_file_name)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct\n");
        return;
    }

    char* const output_buf =
        (char* const) calloc (tree->n_elem, MAX_NODE_OUTPUT_LEN);
    if (!output_buf)
    {
        perror ("output_buf allocation error");
        return;
    }

    int32_t output_index = 0;

    PrintInPreOrder (tree->root, output_buf, &output_index);

    FILE* tree_out = fopen (out_file_name, "wb");
    if (!tree_out)
    {
        perror ("tree_out fopen() error");
        free (output_buf);

        return;
    }

    fprintf (tree_out, "%s\n", output_buf);

    free (output_buf);
    fclose (tree_out);
}

static void
PrintInPreOrder (const BinTree_node* const node,
                       char*         const output_buf,
                       int32_t*       const output_index)
{
    assert (output_index);
    assert (output_buf);

    if (!node)
    {
        output_buf [(*output_index)++] = '_';
        output_buf [(*output_index)++] = ' ';
        return;
    }

    output_buf [(*output_index)++] = '(';
    output_buf [(*output_index)++] = ' ';

    *output_index += snprintf (output_buf + *output_index,
                               PRINT_OUTPUT_ELEM_MAX_LEN,
                               "%u ", node -> data .data_type);

    switch (node -> data .data_type)
    {
        case PUNCTUATION:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN,
                                       "%u ", node -> data .punct_op_code);
            break;

        case BIN_OP:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN,
                                       "%u ", node -> data .bin_op_code);
            break;

        case UN_OP:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN,
                                       "%u ", node -> data .un_op_code);
            break;

        case KEY_OP:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN,
                                       "%u ", node -> data .key_op_code);
            break;

        case NUMBER:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN,
                                       "%lg ", node -> data .num_value);
            break;

        case VARIABLE:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN,
                                       "%zd ", node -> data .var_index);
            break;

        case FUNCTION:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN,
                                       "%zd ", node -> data .func_index);
            break;

        case NO_TYPE:
            *output_index += snprintf (output_buf + *output_index,
                                       PRINT_OUTPUT_ELEM_MAX_LEN, "ERROR");
            break;
    }

    PrintInPreOrder (node -> left,  output_buf, output_index);

    PrintInPreOrder (node -> right, output_buf, output_index);

    output_buf [(*output_index)++] = ')';
    output_buf [(*output_index)++] = ' ';
}
