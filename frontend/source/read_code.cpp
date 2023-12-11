#include "read_code.h"
#include "List_commands.h"

/*
 * Here is the description of grammar rules of the code.
 *
 * G     stands for GetGrammar   (),
 * Op    stands for GetOperation (),
 * A     stands for GetAssume    (),
 * If    stands for GetIf        (),
 * While stands for GetWhile     (),
 * E     stands for GetExpression(),
 * T     stands for GetTerm      (),
 * P     stands for GetPrimary   (),
 * V     stands for GetValue     (),
 * N     stands for GetNumber    (),
 * Var   stands for GetVariable  ().
 *
 * G     ::= Op '\0'
 * Op    ::= A | If | While "Precious"
 * A     ::= "Give him" Var "A pony" E
 * If    ::= "One does not simply walk into Mordor" P "Black" Op+ "Gates"
 * While ::= "So it begins" P "Black" Op+ "Gates"
 *
 * E     ::= T {[+ -] T}*
 * T     ::= U {[* / ^] U}*
 * P     ::= "Unexpected" E "Journey" | V
 * V     ::= N | Var
 * N     ::= [0 - 9]+ {.[0 - 9]+}?
 * Var   ::= {[A - Z] | [a - z]} {[A - Z] | [a - z] | [0 - 9]}*
 */

static void
SeparateToTokens (const file_input* const input_parsed,
                        List*       const tokens_list);

static void
GetTokenData (const file_input* const input_parsed,
                    size_t*     const index,
                    token*      const cur_token);

static BinTree_node*
GetGrammar (const List*    const tokens_list,
                  size_t*  const token_index,
                  BinTree* const tree);

#undef GrammarParams

/**
 * Define is used to avoid too long calls for struct members.
*/
#define GrammarParams                           \
    const List_data_type* const tokens_array,   \
          size_t*         const token_index,    \
          BinTree*        const tree

static BinTree_node*
GetOperation  (GrammarParams);

static BinTree_node*
GetAssume     (GrammarParams);

static BinTree_node*
GetIf         (GrammarParams);

static BinTree_node*
GetWhile      (GrammarParams);

static BinTree_node*
GetExpression (GrammarParams);

static BinTree_node*
GetTerm       (GrammarParams);

static BinTree_node*
GetPrimary    (GrammarParams);

static BinTree_node*
GetValue      (GrammarParams);

static BinTree_node*
GetVariable   (GrammarParams);

static void
syn_assert_func (const size_t n_line,
                 const bool expression);

#define syn_assert(expression)              \
    syn_assert_func (__LINE__, expression);

#define params_assert       \
    assert (tokens_array);  \
    assert (token_index);   \
    assert (tree);

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct\n");
        return nullptr;
    }

    file_input input_parsed = {};
    GetFileInput (input_file_name, &input_parsed, NOT_PARTED);
    input_parsed .buffer [input_parsed .buffer_size - 1] = NULL_TERMINATOR;

    if (input_parsed .buffer_size == 0) return nullptr;

    List tokens_list = {};
    List_Ctor (&tokens_list);

    SeparateToTokens (&input_parsed, &tokens_list);

    FreeFileInput    (&input_parsed);

    size_t token_index = 0;
    tree->root = GetGrammar (&tokens_list, &token_index, tree);

    /* null-termination check */
    syn_assert (tokens_list.list_data [token_index]
                .token_data_type == OPERATION &&
                tokens_list.list_data [token_index]
                .data.op_code == NULL_TERMINATOR);

    SetParents (nullptr, tree -> root);

    List_Dtor (&tokens_list);

    return tree;
}

static void
SeparateToTokens (const file_input* const input_parsed,
                        List*       const tokens_list)
{
    assert (input_parsed);
    assert (input_parsed -> buffer);
    assert (tokens_list);

    size_t index = 0;

    token cur_token = {.token_data_type = NUMBER,
                       .data.num_value  = BinTree_POISON};

    while (index < input_parsed -> buffer_size)
    {
        if (isspace (input_parsed -> buffer [index]))
        {
            index++;
            continue;
        }

        GetTokenData (input_parsed, &index, &cur_token);

        List_PushBack (&cur_token, tokens_list);
    }
}

static void
GetTokenData (const file_input* const input_parsed,
                    size_t*     const index,
                    token*      const cur_token)
{
    assert (input_parsed);
    assert (index);
    assert (cur_token);

    if (input_parsed -> buffer [*index] == NULL_TERMINATOR)
    {
        (*index)++;
        cur_token -> token_data_type = OPERATION;
        cur_token -> data .op_code   = NULL_TERMINATOR;

        return;
    }

    for (op_code_type key_word = 0;
                      key_word < NUM_OF_KEY_WORDS;
                      key_word++)
    {
        size_t key_word_len = strlen (KEY_WORDS_ARRAY [key_word]);

        if (key_word_len != 0 &&
            strncasecmp (input_parsed -> buffer + *index,
                         KEY_WORDS_ARRAY [key_word],
                         key_word_len) == 0)
        {
            (*index) += key_word_len;
            cur_token -> token_data_type = OPERATION;
            cur_token -> data .op_code   = key_word;

            return;
        }
    }

    if (isnumber (input_parsed -> buffer [*index]))
    {
        int32_t n_read_elem = 0;

        sscanf (input_parsed -> buffer + *index, "%lg%n",
                &(cur_token -> data .num_value), &n_read_elem);

        *index += (size_t) n_read_elem;

        cur_token -> token_data_type = NUMBER;
        return;
    }

    else
    {
        size_t str_length = 0;

        while (isalnum (input_parsed -> buffer [*index]))
        {
            cur_token -> data .var_name [str_length++] =
                input_parsed -> buffer  [(*index)++];
        }

        cur_token -> data .var_name [str_length] = NULL_TERMINATOR;

        cur_token -> token_data_type = VARIABLE;
    }
}

static BinTree_node*
GetGrammar (const List*    const tokens_list,
                  size_t*  const token_index,
                  BinTree* const tree)
{
    assert (tokens_list);
    assert (token_index);
    assert (tree);

    /* token_index start value not zero because of dummy_elem in list */
    *token_index = 1;
    return GetOperation (tokens_list -> list_data,
                         token_index, tree);
}

static BinTree_node*
GetOperation (GrammarParams)
{
    params_assert;

    syn_assert (tokens_array [*token_index]
                .token_data_type == OPERATION);

    BinTree_node* new_node      = nullptr;
    BinTree_node* ret_node      = nullptr;
    BinTree_node* cur_separator = nullptr;

    op_code_type cur_op_code = tokens_array [*token_index] .data .op_code;

    while (cur_op_code == ASSUME_BEGIN ||
           cur_op_code == IF           ||
           cur_op_code == WHILE)
    {
        switch (tokens_array [*token_index] .data .op_code)
        {
            case ASSUME_BEGIN:
                (*token_index)++;
                new_node = GetAssume (tokens_array, token_index, tree);
                break;

            case IF:
                (*token_index)++;
                new_node = GetIf (tokens_array, token_index, tree);
                break;

            case WHILE:
                (*token_index)++;
                new_node = GetWhile (tokens_array, token_index, tree);
                break;

            default:
                syn_assert (0);
                return nullptr;
        }

        syn_assert (tokens_array [*token_index] .data
                    .op_code == END_OF_OPERATION);
        (*token_index)++;

        if (!ret_node)
        {
            ret_node = BinTree_CtorNode (OPERATION, END_OF_OPERATION,
                                         new_node, nullptr, nullptr, tree);
            cur_separator = ret_node;
        }

        else
        {
            cur_separator -> right =
                BinTree_CtorNode (OPERATION, END_OF_OPERATION,
                                  new_node, nullptr, nullptr, tree);

            cur_separator = cur_separator -> right;
        }

        cur_op_code = tokens_array [*token_index] .data .op_code;
    }

    return ret_node;
}

static BinTree_node*
GetAssume (GrammarParams)
{
    params_assert;

    BinTree_node* left_value  = GetVariable  (tokens_array, token_index, tree);

    syn_assert (tokens_array[*token_index] .data .op_code == ASSUME_END);
    (*token_index)++;

    BinTree_node* right_value = GetExpression (tokens_array, token_index, tree);

    return BinTree_CtorNode (OPERATION, ASSUME_BEGIN, left_value, right_value,
                             nullptr, tree);
}

static BinTree_node*
GetIf (GrammarParams)
{
    params_assert;

    BinTree_node* left_value = GetPrimary (tokens_array, token_index, tree);

    syn_assert (tokens_array [*token_index] .data .op_code == OPEN_BRACES);
    (*token_index)++;

    BinTree_node* right_value = GetOperation (tokens_array, token_index, tree);

    syn_assert (tokens_array [*token_index] .data .op_code == CLOSE_BRASES);
    (*token_index)++;

    return BinTree_CtorNode (OPERATION, IF, left_value, right_value,
                             nullptr, tree);
}

static BinTree_node*
GetWhile (GrammarParams)
{
    params_assert;

    BinTree_node* left_value = GetPrimary (tokens_array, token_index, tree);

    syn_assert (tokens_array [*token_index] .data .op_code == OPEN_BRACES);
    (*token_index)++;

    BinTree_node* right_value = GetOperation (tokens_array, token_index, tree);

    syn_assert (tokens_array [*token_index] .data .op_code == CLOSE_BRASES);
    (*token_index)++;

    return BinTree_CtorNode (OPERATION, WHILE, left_value, right_value,
                             nullptr, tree);
}

static BinTree_node*
GetExpression (GrammarParams)
{
    params_assert;

    BinTree_node* left_value  = GetTerm (tokens_array, token_index, tree);
    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    bool is_operation = tokens_array [*token_index]
                       .token_data_type == OPERATION;

    bool is_add       = tokens_array [*token_index]
                       .data .op_code   == ADD;

    bool is_sub       = tokens_array [*token_index]
                       .data .op_code   == SUB;

    while (is_operation && (is_add || is_sub))
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .data .op_code;

        right_value = GetTerm (tokens_array, token_index, tree);

        new_node = BinTree_CtorNode (OPERATION, op_code, left_value,
                                     right_value, nullptr, tree);

        left_value = new_node;

        is_operation = tokens_array [*token_index]
                      .token_data_type == OPERATION;

        is_add       = tokens_array [*token_index]
                      .data .op_code   == ADD;

        is_sub       = tokens_array [*token_index]
                      .data .op_code   == SUB;
    }

    return left_value;
}

static BinTree_node*
GetTerm (GrammarParams)
{
    params_assert;

    BinTree_node* left_value  = GetPrimary (tokens_array, token_index, tree);
    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    bool is_operation = tokens_array [*token_index]
                       .token_data_type == OPERATION;

    bool is_mul       = tokens_array [*token_index]
                       .data .op_code   == MUL;

    bool is_div       = tokens_array [*token_index]
                       .data .op_code   == DIV;

    while (is_operation && (is_mul || is_div))
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .data .op_code;

        right_value = GetPrimary (tokens_array, token_index, tree);

        new_node = BinTree_CtorNode (OPERATION, op_code, left_value,
                                     right_value, nullptr, tree);

        left_value = new_node;

        is_operation = tokens_array [*token_index]
                      .token_data_type == OPERATION;

        is_mul       = tokens_array [*token_index]
                      .data .op_code   == MUL;

        is_div       = tokens_array [*token_index]
                      .data .op_code   == DIV;
    }

    return left_value;
}

static BinTree_node*
GetPrimary (GrammarParams)
{
    params_assert;

    BinTree_node* node = nullptr;

    if (tokens_array [*token_index] .token_data_type == OPERATION &&
        tokens_array [*token_index] .data .op_code   == OPEN_PARENTHESIS)
    {
        (*token_index)++;

        node = GetExpression (tokens_array, token_index, tree);

        syn_assert
        (
            tokens_array [*token_index] .token_data_type == OPERATION &&
            tokens_array [*token_index] .data .op_code   == CLOSE_PARENTHESIS
        );

        (*token_index)++;

        return node;
    }

    return GetValue (tokens_array, token_index, tree);
}

static BinTree_node*
GetValue (GrammarParams)
{
    params_assert;

    switch (tokens_array [*token_index] .token_data_type)
    {
        case NUMBER:
            return BinTree_CtorNode (NUMBER,
                                     tokens_array [(*token_index)++]
                                     .data .num_value,
                                     nullptr, nullptr, nullptr, tree);
        case VARIABLE:
            return GetVariable (tokens_array, token_index, tree);

        case OPERATION:
            [[fallthrough]];

        case NO_TYPE:
            [[fallthrough]];

        default:
            fprintf (stderr, "\n\n%zd\n", *token_index);
            syn_assert (NO_TYPE);
            return nullptr;
    }
}

static BinTree_node*
GetVariable (GrammarParams)
{
    params_assert;

    char* const var_name =
        (char*) calloc (VAR_NAME_MAX_LEN, sizeof (char));
    if (!var_name)
    {
        perror ("var_name allocation error");
        return nullptr;
    }

    strcpy (var_name, tokens_array [(*token_index)++] .data .var_name);

    for (var_index_type i = 0; i < tree -> var_number; ++i)
    {
        if (strcmp (var_name,
                    tree -> name_table .var_table [i] .var_name) == 0)
        {
            return BinTree_CtorNode (VARIABLE, i, nullptr,
                                     nullptr, nullptr, tree);
        }
    }

    if (tree -> var_number == tree -> var_table_capacity)
    {
        tree -> var_table_capacity *= VAR_TABLE_CAPACITY_MULTIPLIER;
        ReallocVarTable (tree);
    }

    tree -> name_table .var_table [tree->var_number] .var_name = var_name;

    return BinTree_CtorNode (VARIABLE, tree -> var_number++,
                             nullptr, nullptr, nullptr, tree);
}

static void
syn_assert_func (const size_t n_line,
                 const bool expression)
{
    if (!expression)
    {
        fprintf (stderr, "Aborting line %zd\n", n_line);
        abort ();
    }
}

#undef GrammarParams
#undef syn_assert
#undef param_assert
