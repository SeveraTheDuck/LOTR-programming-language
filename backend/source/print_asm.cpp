#include "print_asm.h"

static const char* const asm_op_array [NUM_OF_KEY_WORDS] =
    {
     "SIN", "COS", "SQRT", "LN", "!", "OUT", "OUT_S", "IN",
     "return_value",

     "ADD", "SUB", "MUL", "DIV", "POW",
     "IS_EQUAL", "GREATER", "LESS", "GOE", "LOE", "NOT_EQUAL",
     "=",

     "IF",
     "WHILE",

     ";"
    };

#define IF_TRUE_LABEL     ":if_true_label%d\n"
#define IF_FALSE_LABEL    ":if_false_label%d\n"

#define WHILE_TRUE_LABEL  ":while_true_label%d\n"
#define WHILE_FALSE_LABEL ":while_false_label%d\n"

#define FUNC_LABEL        ":func%zd\n"

static void
PrintMainFunction       (const BinTree_node* const node);

static void
PrintFunction           (const BinTree_node* const node);

static void
PrintFunctionFormalArgs (const BinTree_node* const node);

static void
PrintNodeToAsm          (const BinTree_node* const node,
                         const bool is_in_operation);

static void
PrintIfOperation        (const BinTree_node* const node);

static void
PrintWhileOperation     (const BinTree_node* const node);

static void
PrintRetOperation       (const BinTree_node* const node);

static void
PrintFunctionCall       (const BinTree_node* const node,
                         const bool is_in_operation);

static void
PushSavingVariables     (const BinTree_node* const node);

static void
PopSavingVariables      (const BinTree_node* const node);

static void
PrintFactualArguments   (const BinTree_node* const node);

void
PrintTreeToAsm (const BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct.\n");
        return;
    }

    PrintMainFunction (tree -> root);
}

static void
PrintMainFunction (const BinTree_node* const node)
{
    assert (node);

    printf ("\t\tjmp :main\n\n");

    PrintFunction (node -> right);

    printf (":main\n");

    PrintNodeToAsm (node -> left, NOT_IN_OPERATION);

    printf ("\t\thlt\n");
}

static void
PrintFunction (const BinTree_node* const node)
{
    if (!node) return;

    static size_t func_number = FIRST_FUNC_NUMBER;

    printf (FUNC_LABEL, func_number++);

    PrintFunctionFormalArgs (node -> left -> right);

    PrintNodeToAsm          (node -> left -> left, NOT_IN_OPERATION);

    printf ("\t\tret\n\n");

    PrintFunction (node -> right);
}

static void
PrintFunctionFormalArgs (const BinTree_node* const node)
{
    if (!node) return;

    PrintFunctionFormalArgs (node -> right);

    printf ("\t\tPOP [%zd]\n", node -> left -> data .var_index);
}

static void
PrintNodeToAsm (const BinTree_node* const node,
                const bool is_in_operation)
{
    if (!node) return;

    switch (node -> data .data_type)
    {
        case PUNCTUATION:
        {
            PrintNodeToAsm (node -> left,  is_in_operation);
            PrintNodeToAsm (node -> right, is_in_operation);

            break;
        }

        case BIN_OP:
        {
            if (node -> data .bin_op_code == ASSUME_BEGIN)
            {
                PrintNodeToAsm (node -> right, IN_OPERATION);
                printf ("\t\tPOP [%zd]\n", node -> left -> data .var_index);
            }

            else
            {
                PrintNodeToAsm (node -> left,  IN_OPERATION);
                PrintNodeToAsm (node -> right, IN_OPERATION);

                printf ("\t\t%s\n", asm_op_array [node -> data .bin_op_code]);
            }

            break;
        }

        case UN_OP:
        {
            if (node -> data .un_op_code == RET)
            {
                PrintRetOperation (node);
            }

            else if (node -> data .un_op_code == IN)
            {
                printf ("\t\t%s\n", asm_op_array [node -> data .un_op_code]);
                printf ("\t\tPOP [%zd]\n", node -> right -> data .var_index);
            }

            else
            {
                PrintNodeToAsm (node -> right, IN_OPERATION);
                printf ("\t\t%s\n", asm_op_array [node -> data .un_op_code]);
            }

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
            printf ("\t\tPUSH [%zd]\n", node -> data .var_index);
            break;
        }

        case FUNCTION:
        {
            PrintFunctionCall (node, is_in_operation);
            break;
        }

        case NO_TYPE:
        {
            [[fallthrough]];
        }

        default:
        {
            exit (1); //AAA
        }
    }
}

static void
PrintIfOperation (const BinTree_node* const node)
{
    assert (node);

    // to struct
    static int8_t if_number = 0;
    const  int8_t cur_if_number = if_number;

    PrintNodeToAsm (node -> left, IN_OPERATION);
    printf ("\t\tPUSH 0\n");
    printf ("\t\tje " IF_FALSE_LABEL, cur_if_number);
    if_number++;

    PrintNodeToAsm (node -> right -> left, IN_OPERATION);
    printf ("\t\tjmp " IF_TRUE_LABEL, cur_if_number);

    printf (IF_FALSE_LABEL, cur_if_number);
    PrintNodeToAsm (node -> right -> right, IN_OPERATION);

    printf (IF_TRUE_LABEL, cur_if_number);
}

static void
PrintWhileOperation (const BinTree_node* const node)
{
    assert (node);

    // to struct
    static int8_t while_number = 0;
    const  int8_t cur_while_number = while_number;

    printf (WHILE_TRUE_LABEL, cur_while_number);

    PrintNodeToAsm (node -> left, IN_OPERATION);
    printf ("\t\tPUSH 0\n");
    printf ("\t\tje " WHILE_FALSE_LABEL, cur_while_number);
    while_number++;

    PrintNodeToAsm (node -> right -> left, IN_OPERATION);
    printf ("\t\tjmp " WHILE_TRUE_LABEL, cur_while_number);

    PrintNodeToAsm (node -> right -> right, IN_OPERATION);

    printf (WHILE_FALSE_LABEL, cur_while_number);
}

static void
PrintRetOperation (const BinTree_node* const node)
{
    assert (node);

    PrintNodeToAsm (node -> right, IN_OPERATION);
    printf ("\t\tPOP rax\n\t\tret\n");
}

static void
PrintFunctionCall (const BinTree_node* const node,
                   const bool is_in_operation)
{
    assert (node);

    PushSavingVariables   (node -> right);
    PrintFactualArguments (node -> right);

    printf ("\t\tcall " FUNC_LABEL, node -> data .func_index);

    PopSavingVariables    (node -> right);

    if (is_in_operation)
    {
        printf ("\t\tPUSH rax\n");
    }
}

static void
PushSavingVariables (const BinTree_node* const node)
{
    if (!node) return;

    if (node -> data .data_type == VARIABLE)
    {
        printf ("\t\tPUSH [%zd]\n", node -> data .var_index);
    }

    PushSavingVariables (node -> left);
    PushSavingVariables (node -> right);
}

static void
PopSavingVariables (const BinTree_node* const node)
{
    if (!node) return;

    PopSavingVariables (node -> right);
    PopSavingVariables (node -> left);

    if (node -> data .data_type == VARIABLE)
    {
        printf ("\t\tPOP [%zd]\n", node -> data .var_index);
    }
}

static void
PrintFactualArguments (const BinTree_node* const node)
{
    if (!node) return;

    PrintNodeToAsm (node -> left, IN_OPERATION);
    PrintFactualArguments (node -> right);
}
