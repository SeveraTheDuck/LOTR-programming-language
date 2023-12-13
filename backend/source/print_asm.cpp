#include "print_asm.h"

/* error strings are used to fill space in enum values and catch errors*/
static const char* asm_op_array[] =
    {
     "ERROR_NULL_TREMINATOR",   "ERROR_OPEN_PARENTHESIS",
     "ERROR_CLOSE_PARENTHESIS", "ERROR_OPEN_BRACKET",
     "ERROR_CLOSE_BRACKET",

     ";", "ADD", "SUB", "MUL", "DIV", "POW",
     "==", ">", "<", ">=", "<=", "!=", "=",

     "ERROR_ASSUME_SECOND_PART",
     "SIN", "COS", "SQRT", "LN", "!", "OUT", "OUT_S", "IN",
     "if",
     "while"
    };

#define IF_TRUE_LABEL     ":if_true_label%d\n"
#define IF_FALSE_LABEL    ":if_false_label%d\n"

#define WHILE_TRUE_LABEL  ":while_true_label%d\n"
#define WHILE_FALSE_LABEL ":while_false_label%d\n"

static void
PrintNodeToAsm      (const BinTree_node* const node);

static void
PrintIfOperation    (const BinTree_node* const node);

static void
PrintWhileOperation (const BinTree_node* const node);

void
PrintTreeToAsm (const BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct.\n");
        return;
    }

    PrintNodeToAsm (tree -> root);
}

static void
PrintNodeToAsm (const BinTree_node* const node)
{
    if (!node) return;

    switch (node -> data .data_type)
    {
        case PUNCTUATION:
        {
            PrintNodeToAsm (node -> left);
            PrintNodeToAsm (node -> right);

            break;
        }

        case BIN_OP:
        {
            if (node -> data .bin_op_code == ASSUME_BEGIN)
            {
                PrintNodeToAsm (node -> right);
                printf ("\t\tPOP [%d]\n", node -> left -> data .var_index);
            }

            else
            {
                PrintNodeToAsm (node -> left);
                PrintNodeToAsm (node -> right);

                int32_t shift = NUM_OF_PUNCT_SYMBOLS;
                printf ("\t\t%s\n", asm_op_array
                                    [node -> data .bin_op_code + shift]);
            }

            break;
        }

        case UN_OP:
        {
            PrintNodeToAsm (node -> right);

            int32_t shift = NUM_OF_PUNCT_SYMBOLS + NUM_OF_BIN_OP;
            printf ("\t\t%s\n", asm_op_array
                                [node -> data .un_op_code + shift]);

            break;
        }

        case KEY_OP:
        {
            switch (node -> data .key_op_code)
            {
                case IF:
                    PrintIfOperation    (node);
                    break;

                case WHILE:
                    PrintWhileOperation (node);
                    break;
            }

            break;
        }

        case NUMBER:
        {
            printf ("\t\tPUSH %lg\n", node -> data .num_value);
            break;
        }

        case VARIABLE:
        {
            printf ("\t\tPUSH [%d]\n", node -> data .var_index);
            break;
        }

        case NO_TYPE:
        {
            [[fallthrough]];
        }

        default:
        {
            abort ();
        }
    }
}

static void
PrintIfOperation (const BinTree_node* const node)
{
    assert (node);

    static int8_t if_number = 0;
    const  int8_t cur_if_number = if_number;

    PrintNodeToAsm (node -> left);
    printf ("\t\tPUSH 0\n");
    printf ("\t\tje " IF_FALSE_LABEL, cur_if_number);
    if_number++;

    PrintNodeToAsm (node -> right -> left);
    printf ("\t\tjmp " IF_TRUE_LABEL, cur_if_number);

    printf (IF_FALSE_LABEL, cur_if_number);
    PrintNodeToAsm (node -> right -> right);

    printf (IF_TRUE_LABEL, cur_if_number);
}

static void
PrintWhileOperation (const BinTree_node* const node)
{
    assert (node);

    static int8_t while_number = 0;
    const  int8_t cur_while_number = while_number;

    printf (WHILE_TRUE_LABEL, cur_while_number);

    PrintNodeToAsm (node -> left);
    printf ("\t\tPUSH 0\n");
    printf ("\t\tje " WHILE_FALSE_LABEL, cur_while_number);
    while_number++;

    PrintNodeToAsm (node -> right -> left);
    printf ("\t\tjmp " WHILE_TRUE_LABEL, cur_while_number);

    PrintNodeToAsm (node -> right -> right);

    printf (WHILE_FALSE_LABEL, cur_while_number);
}
