// #include "BinTree_PrintPostOrder.h"
//
// static void
// PrintInPostOrder (const BinTree_node* const node,
//                         char*         const output_buf);
//
// void
// PrintTreeToFile (const BinTree* const tree)
// {
//     if (!tree)
//     {
//         fprintf (stderr, "Invalid pointer to tree struct\n");
//         return;
//     }
//
//     /* count = ?? */
//     char* const output_buf =
//         (char* const) calloc (tree->n_elem, sizeof (double));
//     if (!output_buf)
//     {
//         perror ("output_buf allocation error");
//         return;
//     }
//
//     PrintInPostOrder (tree->root, output_buf);
// }
//
// static void
// PrintInPostOrder (const BinTree_node* const node,
//                         char*         const output_buf)
// {
//     assert (output_buf);
//     if (!node) return;
//
//     sputc ('(', output_buf);
//     sputc (' ', output_buf);
//
//     PrintInPostOrder (node -> left,  output_buf);
//
//     PrintInPostOrder (node -> right, output_buf);
//
//     snprintf (output_buf, PRINTF_OUTPUT_MAX_LEN,
//               "%u ", node -> data .data_type);
//
//     switch (node -> data .data_type)
//     {
//         case NUMBER:
//             snprintf (output_buf, PRINTF_OUTPUT_MAX_LEN, "%lg ",
//                      node -> data .data_value .num_value);
//             break;
//
//         case OPERATION:
//             snprintf (output_buf, PRINTF_OUTPUT_MAX_LEN, "%u ",
//                      node -> data .data_value .op_code);
//             break;
//
//         case VARIABLE:
//             snprintf (output_buf, PRINTF_OUTPUT_MAX_LEN, "%hhd ",
//                      node -> data .data_value .var_index);
//             break;
//
//         case NO_TYPE:
//             snprintf (output_buf, PRINTF_OUTPUT_MAX_LEN, "ERROR");
//             break;
//     }
//
//     sputc (')', stdout);
//     sputc (' ', stdout);
// }
