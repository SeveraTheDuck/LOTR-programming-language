#include "read_code.h"
#include "List_commands.h"

/*
 * Here is the description of grammar rules of the code.
 *
 * G     stands for GetGrammar      (),
 * MF    stands for mazafaka (MultipleFunctions),
 * F     stands for GetFunction     (),
 * FArgs stands for GetFunctionArgs (),
 * Op    stands for GetOperation    (),
 * A     stands for GetAssume       (),
 * If    stands for GetIf           (),
 * While stands for GetWhile        (),
 * B     stands for GetBody         (),
 * C     stands for GetComparison   (),
 * E     stands for GetExpression   (),
 * T     stands for GetTerm         (),
 * P     stands for GetPrimary      (),
 * V     stands for GetValue        (),
 * N     stands for GetNumber       (),
 * Var   stands for GetVariable     ().
 *
 * G     ::= MF '\0'
 * MF    ::= F+
 * F     ::= "Mellon" {FArgs}? B
 * FArgs ::= "Fellowship" E {, E}* "of the Ring"
 * B     ::= "Black" Op+ "Gates"
 * Op    ::= Call | A | If | While "Precious"
 * A     ::= "Give him" Var "A pony" E
 * If    ::= "One does not simply walk into Mordor" P B B?
 * While ::= "So it begins" P B
 *
 * C     ::= E {[== > < >= <= !=] E}?
 * E     ::= T {[+ -] T}*
 * T     ::= U {[* / ^] U}*
 * P     ::= "Unexpected" E "Journey" | V
 * V     ::= N | Var
 * N     ::= [+ -]? {[0 - 9]+ {.[0 - 9]*}?} | {[0 - 9]* {.[0 - 9]+}}
 * Var   ::= {[A - Z] | [a - z]} {[A - Z] | [a - z] | [0 - 9]}*
 */

#undef GrammarParams
#undef GiveParams
#undef params_assert
#undef syn_assert
#undef IsBinOperation
#undef IsUnOperation
#undef IsKeyOperation
#undef IsFunction
#undef IsVariable
#undef IsPunctuation



/* LEXICAL ANALYSIS BEGIN */

static void
SeparateToTokens (const char*    const input_file_name,
                        List*    const tokens_list,
                        BinTree* const tree);

static void
GetTokenData (const file_input* const input_parsed,
                    size_t*     const index,
                    token*      const cur_token);

static void
GetTokenOperationType (      token* const cur_token,
                       const op_code_type key_word);

static BinTree_node*
GetGrammar (const List*    const tokens_list,
                  size_t*  const token_index,
                  BinTree* const tree);

static void
GetFunctionNames (const List*    const tokens_list,
                        BinTree* const tree);

static void
SetFunctionNames (const List*    const tokens_list,
                         BinTree* const tree);

/* LEXICAL ANALYSIS END */



/* RECURSIVE DESCENT BEGIN */

/*
 * Defines are used to avoid too long calls for struct members.
 */
#define GrammarParams                           \
    const List_data_type* const tokens_array,   \
          size_t*         const token_index,    \
          BinTree*        const tree

#define GiveParams tokens_array, token_index, tree

#define params_assert       \
    assert (tokens_array);  \
    assert (token_index);   \
    assert (tree);

static BinTree_node*
GetMultipleFunctions (GrammarParams);

static BinTree_node*
GetFunction          (GrammarParams);

static var_index_type
GetFunctionIndex     (GrammarParams);

static BinTree_node*
GetFunctionArgs      (GrammarParams);

static BinTree_node*
GetOperation         (GrammarParams);

static BinTree_node*
GetAssume            (GrammarParams);

static BinTree_node*
GetIf                (GrammarParams);

static BinTree_node*
GetWhile             (GrammarParams);

static BinTree_node*
GetBody              (GrammarParams);

static BinTree_node*
GetComparison        (GrammarParams);

static BinTree_node*
GetExpression        (GrammarParams);

static BinTree_node*
GetTerm              (GrammarParams);

static BinTree_node*
GetPrimary           (GrammarParams);

static BinTree_node*
GetValue             (GrammarParams);

static BinTree_node*
GetVariable          (GrammarParams);

/* RECURSIVE DESCENT END */



/* HELPING FUNCTION AND DEFINES BEGIN */

static inline void
syn_assert_func (const size_t n_line,
                 const bool expression);

#define syn_assert(expression)                      \
    syn_assert_func (__LINE__, expression);

static inline bool
IsType (const List_data_type* const tokens_array,
        const size_t                token_index,
        const data_type             type);

#define IsBinOperation(tokens_array, token_index)   \
    IsType (tokens_array, token_index, BIN_OP)

#define IsUnOperation(tokens_array, token_index)    \
    IsType (tokens_array, token_index, UN_OP)

#define IsKeyOperation(tokens_array, token_index)   \
    IsType (tokens_array, token_index, KEY_OP)

#define IsFunction(tokens_array, token_index)       \
    IsType (tokens_array, token_index, FUNCTION)

#define IsVariable(tokens_array, token_index)       \
    IsType (tokens_array, token_index, VARIABLE)

#define IsPunctuation(tokens_array, token_index)    \
    IsType (tokens_array, token_index, PUNCTUATION)

/* HELPING FUNCTION AND DEFINES END */



BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct\n");
        return nullptr;
    }

    List tokens_list = {};
    List_Ctor (&tokens_list);
    tokens_list .list_data [List_DUMMY_ELEMENT] .token_data_type = NO_TYPE;

    SeparateToTokens (input_file_name, &tokens_list, tree);

    size_t token_index = 0;
    tree->root = GetGrammar (&tokens_list, &token_index, tree);

    /*
     * Null-termination check
     * Not with defines because of different form of calling
     */
    syn_assert (IsPunctuation (tokens_list .list_data, token_index) &&
                tokens_list.list_data [token_index]
                .punct_op_code == NULL_TERMINATOR);

    SetParents (nullptr, tree -> root);

    List_Dtor (&tokens_list);

    return tree;
}

static void
SeparateToTokens (const char*    const input_file_name,
                        List*    const tokens_list,
                        BinTree* const tree)
{
    assert (tokens_list);
    assert (tree);

    file_input input_parsed = {};

    GetFileInput (input_file_name, &input_parsed, NOT_PARTED);
    input_parsed .buffer [input_parsed .buffer_size - 1] = '\0';

    if (input_parsed .buffer_size == 0) return;

    size_t index = 0;

    token cur_token = {.token_data_type = NUMBER,
                       .num_value  = BinTree_POISON};

    while (index < input_parsed .buffer_size)
    {
        size_t cur_index = index;

        if (isspace (input_parsed .buffer [index]))
        {
            index++;
            continue;
        }

        GetTokenData (&input_parsed, &index, &cur_token);

        syn_assert (cur_index != index);

        List_PushBack (&cur_token, tokens_list);
    }

    FreeFileInput (&input_parsed);

    /*
     * This block of two functions is used to make variables
     * that name functions with FUNCTION data type.
     */

    GetFunctionNames (tokens_list, tree);

    SetFunctionNames (tokens_list, tree);
}

static void
GetTokenData (const file_input* const input_parsed,
                    size_t*     const index,
                    token*      const cur_token)
{
    assert (input_parsed);
    assert (index);
    assert (cur_token);

    if (input_parsed -> buffer [*index] == '\0')
    {
        (*index)++;
        cur_token -> token_data_type = PUNCTUATION;
        cur_token -> punct_op_code   = NULL_TERMINATOR;

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

            GetTokenOperationType (cur_token, key_word);

            return;
        }
    }

    if (isnumber (input_parsed -> buffer [*index]) ||
        input_parsed -> buffer [*index] == '+'     ||
        input_parsed -> buffer [*index] == '-')
    {
        int32_t n_read_elem = 0;

        sscanf (input_parsed -> buffer + *index, "%lg%n",
                &(cur_token  -> num_value), &n_read_elem);

        *index += (size_t) n_read_elem;

        cur_token -> token_data_type = NUMBER;
        return;
    }

    else
    {
        size_t str_length = 0;

        while (isalnum (input_parsed -> buffer [*index]))
        {
            cur_token -> var_name [str_length++] =
                input_parsed -> buffer  [(*index)++];
        }

        cur_token -> var_name [str_length] = '\0';

        cur_token -> token_data_type = VARIABLE;
    }
}

static void
GetTokenOperationType (      token* const cur_token,
                       const op_code_type key_word)
{
    assert (cur_token);

    if (key_word == FUNC_DEF)
    {
        cur_token -> token_data_type = FUNCTION;
        cur_token -> key_op_code     = key_word;
    }

    else if (key_word < NUM_OF_UN_OP)
    {
        cur_token -> token_data_type = UN_OP;
        cur_token -> un_op_code      = key_word;
    }

    else if (key_word < NUM_OF_BIN_OP)
    {
        cur_token -> token_data_type = BIN_OP;
        cur_token -> bin_op_code     = key_word;
    }

    else if (key_word < NUM_OF_KEY_OP)
    {
        cur_token -> token_data_type = KEY_OP;
        cur_token -> key_op_code     = key_word;
    }

    else if (key_word < NUM_OF_KEY_WORDS)
    {
        cur_token -> token_data_type = PUNCTUATION;
        cur_token -> punct_op_code   = key_word;
    }

    else
    {
        fprintf (stderr, "Wrong data type\n");
        return;
    }
}

static void
GetFunctionNames (const List*    const tokens_list,
                        BinTree* const tree)
{
    assert (tokens_list);
    assert (tree);

    for (size_t i = 0; i < tokens_list -> list_n_elems; ++i)
    {
        /*
         * FUNC_DEF word is the only thing that is marked
         * with FUNCTION data type so we could use it here
         */
        if (IsFunction (tokens_list -> list_data, i))
        {
            /* i++ to go from FUNC_DEF to func_name variable */
            i++;
            var_index_type func_index =
                GetFunctionIndex (tokens_list -> list_data, &i, tree);

            /*
             * every function name is a variable during
             * reading, changing it here
             */
            tokens_list -> list_data [i] .token_data_type = FUNCTION;

            if (func_index == VAR_INDEX_POISON)
            {
                char* new_str =
                    (char*) calloc (VAR_NAME_MAX_LEN, sizeof (char));
                if (!new_str)
                {
                    perror ("new_str allocation error");
                    return;
                }

                strcpy (new_str, tokens_list -> list_data [i] .var_name);

                StackPush (tree -> name_table .func_table, new_str);
            }
        }
    }
}

static void
SetFunctionNames (const List*    const tokens_list,
                        BinTree* const tree)
{
    assert (tokens_list);
    assert (tree);

    for (size_t i = 0; i < tokens_list -> list_n_elems; i++)
    {
        if (IsVariable (tokens_list -> list_data, i))
        {
            var_index_type func_index =
                GetFunctionIndex (tokens_list -> list_data, &i, tree);

            if (func_index != VAR_INDEX_POISON)
            {
                tokens_list -> list_data [i] .token_data_type = FUNCTION;
            }
        }
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

    return GetMultipleFunctions (tokens_list -> list_data,
                                 token_index, tree);
}

static BinTree_node*
GetMultipleFunctions (GrammarParams)
{
    params_assert;

    BinTree_node* main_func = GetFunction (GiveParams);

    BinTree_node* cur_func = main_func;

    while (IsFunction (tokens_array, *token_index))
    {
        cur_func -> right = GetFunction (GiveParams);

        cur_func = cur_func -> right;
    }

    return main_func;
}

static BinTree_node*
GetFunction (GrammarParams)
{
    params_assert;

    syn_assert (IsFunction (tokens_array, *token_index));
    (*token_index)++;

    var_index_type cur_func_index = GetFunctionIndex (GiveParams);
    (*token_index)++;

    BinTree_node* func_args = GetFunctionArgs (GiveParams);

    BinTree_node* func_body = GetBody (GiveParams);

    if (cur_func_index == 0)
    {
        return BinTree_CtorNode (FUNCTION, cur_func_index,
                                 func_body, nullptr, nullptr, tree);
    }

    else
    {
        BinTree_node* whole_function =
            BinTree_CtorNode (PUNCTUATION, END_OF_OPERATION,
                              func_body, func_args, nullptr, tree);

        return BinTree_CtorNode (FUNCTION, cur_func_index,
                                 whole_function, nullptr, nullptr, tree);
    }
}

static BinTree_node*
GetFunctionArgs (GrammarParams)
{
    params_assert;

    BinTree_node* ret_node = nullptr;
    BinTree_node* cur_node = nullptr;
    BinTree_node* new_node = nullptr;

    if (IsPunctuation (tokens_array, *token_index) &&
        tokens_array [*token_index] .punct_op_code == FUNC_ARGS_BEGIN)
    {
        (*token_index)++;

        ret_node = BinTree_CtorNode (PUNCTUATION, END_OF_OPERATION,
                                     nullptr, nullptr, nullptr, tree);

        while (tokens_array [*token_index] .punct_op_code != FUNC_ARGS_END)
        {
            new_node = GetExpression (GiveParams);

            if (!cur_node)
            {
                ret_node -> left = new_node;
                cur_node = ret_node;
            }

            else
            {
                cur_node -> right =
                    BinTree_CtorNode (PUNCTUATION, END_OF_OPERATION,
                                      new_node, nullptr, nullptr, tree);
                cur_node = cur_node -> right;
            }

            if (IsPunctuation (tokens_array, *token_index) &&
                tokens_array [*token_index] .punct_op_code == COMMA)
            {
                (*token_index)++;
            }

            else
            {
                break;
            }
        }

        syn_assert (IsPunctuation (tokens_array, *token_index) &&
                    tokens_array [*token_index]
                   .punct_op_code   == FUNC_ARGS_END);

        (*token_index)++;
    }

    return ret_node;
}

static BinTree_node*
GetOperation (GrammarParams)
{
    params_assert;

    op_code_type   cur_op_code    = OP_CODE_POISON;
    var_index_type cur_func_index = VAR_INDEX_POISON;


    BinTree_node* new_node  = nullptr;
    BinTree_node* ret_value = nullptr;

    if (IsUnOperation (tokens_array, *token_index))
    {
        cur_op_code = tokens_array [(*token_index)++] .un_op_code;

        ret_value = GetExpression (GiveParams);
        new_node  = BinTree_CtorNode (UN_OP, cur_op_code, nullptr,
                                      ret_value, nullptr, tree);
    }

    else if (IsKeyOperation (tokens_array, *token_index))
    {
        cur_op_code = tokens_array [(*token_index)++] .key_op_code;

        switch (cur_op_code)
        {
            case IF:
                new_node = GetIf (GiveParams);
                break;

            case WHILE:
                new_node = GetWhile (GiveParams);
                break;

            default:
                syn_assert (0);
        }
    }

    else if (IsBinOperation (tokens_array, *token_index))
    {
        cur_op_code = tokens_array [(*token_index)++] .bin_op_code;
        syn_assert (cur_op_code == ASSUME_BEGIN);

        new_node = GetAssume (GiveParams);
    }

    else if (IsFunction (tokens_array, *token_index))
    {
        cur_func_index = GetFunctionIndex (GiveParams);
        (*token_index)++;

        if (cur_func_index != VAR_INDEX_POISON)
        {
            BinTree_node* func_args = GetFunctionArgs (GiveParams);

            new_node = BinTree_CtorNode (FUNCTION, cur_func_index,
                                         nullptr, func_args, nullptr, tree);
        }
    }

    else
    {
        syn_assert (0);
    }

    syn_assert (IsPunctuation (tokens_array, *token_index) &&
                tokens_array [*token_index]
                .punct_op_code == END_OF_OPERATION);

    (*token_index)++;

    return new_node;
}

static BinTree_node*
GetAssume (GrammarParams)
{
    params_assert;

    BinTree_node* left_value =
        GetVariable  (GiveParams);

    syn_assert (IsPunctuation (tokens_array, *token_index) &&
                tokens_array [*token_index] .un_op_code == ASSUME_END);

    (*token_index)++;

    BinTree_node* right_value =
        GetComparison (GiveParams);

    return BinTree_CtorNode (BIN_OP, ASSUME_BEGIN, left_value,
                             right_value, nullptr, tree);
}

static BinTree_node*
GetIf (GrammarParams)
{
    params_assert;
    BinTree_node* condition =
        GetPrimary (GiveParams);

    BinTree_node* true_part =
        GetBody  (GiveParams);

    BinTree_node* false_part = nullptr;

    if (IsPunctuation (tokens_array, *token_index) &&
        tokens_array [*token_index] .punct_op_code == OPEN_BRACE)
    {
        false_part = GetBody  (GiveParams);
    }

    BinTree_node* body = BinTree_CtorNode (PUNCTUATION, END_OF_OPERATION,
                                           true_part, false_part,
                                           nullptr, tree);

    return BinTree_CtorNode (KEY_OP, IF, condition,
                             body, nullptr, tree);
}

static BinTree_node*
GetWhile (GrammarParams)
{
    params_assert;

    BinTree_node* condition =
        GetPrimary (GiveParams);

    BinTree_node* true_part =
        GetBody  (GiveParams);

    BinTree_node* false_part = nullptr;

    BinTree_node* body = BinTree_CtorNode (PUNCTUATION, END_OF_OPERATION,
                                           true_part, false_part,
                                           nullptr, tree);

    return BinTree_CtorNode (KEY_OP, WHILE, condition,
                             body, nullptr, tree);
}

static BinTree_node*
GetBody (GrammarParams)
{
    params_assert;

    syn_assert (IsPunctuation (tokens_array, *token_index) &&
                tokens_array [*token_index] .punct_op_code == OPEN_BRACE);

    (*token_index)++;

    BinTree_node* ret_node      = nullptr;
    BinTree_node* cur_separator = nullptr;
    BinTree_node* new_node      = nullptr;

    while (IsBinOperation (tokens_array, *token_index) ||
           IsUnOperation  (tokens_array, *token_index) ||
           IsKeyOperation (tokens_array, *token_index) ||
           IsFunction     (tokens_array, *token_index))
    {
        new_node = GetOperation (GiveParams);

        if (!ret_node)
        {
            ret_node =
                BinTree_CtorNode (PUNCTUATION, END_OF_OPERATION,
                                  new_node, nullptr, nullptr, tree);
            cur_separator = ret_node;
        }

        else
        {
            cur_separator -> right =
                BinTree_CtorNode (PUNCTUATION, END_OF_OPERATION,
                                  new_node, nullptr, nullptr, tree);

            cur_separator = cur_separator -> right;
        }
    }

    syn_assert (IsPunctuation (tokens_array, *token_index) &&
                tokens_array [*token_index] .punct_op_code == CLOSE_BRACE);

    (*token_index)++;

    return ret_node;
}

static BinTree_node*
GetComparison (GrammarParams)
{
    params_assert;

    BinTree_node* left_value  = GetExpression (GiveParams);

    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    bool is_bin_operation = IsBinOperation (tokens_array, *token_index);

    if (!is_bin_operation) return left_value;

    bool is_comparison_sign =
        tokens_array [*token_index] .bin_op_code == IS_EQUAL         ||
        tokens_array [*token_index] .bin_op_code == GREATER          ||
        tokens_array [*token_index] .bin_op_code == LESS             ||
        tokens_array [*token_index] .bin_op_code == GREATER_OR_EQUAL ||
        tokens_array [*token_index] .bin_op_code == LESS_OR_EQUAL    ||
        tokens_array [*token_index] .bin_op_code == NOT_EQUAL;

    if (is_bin_operation && is_comparison_sign)
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .bin_op_code;

        right_value = GetExpression (GiveParams);

        new_node = BinTree_CtorNode (BIN_OP, op_code, left_value,
                                     right_value, nullptr, tree);

        left_value = new_node;
    }

    return left_value;
}

static BinTree_node*
GetExpression (GrammarParams)
{
    params_assert;

    BinTree_node* left_value  = GetTerm (GiveParams);

    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    bool is_bin_operation = IsBinOperation (tokens_array, *token_index);

    if (!is_bin_operation) return left_value;

    bool is_add = tokens_array [*token_index] .bin_op_code == ADD;

    bool is_sub = tokens_array [*token_index] .bin_op_code == SUB;

    while (is_bin_operation && (is_add || is_sub))
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .bin_op_code;

        right_value = GetTerm (GiveParams);

        new_node = BinTree_CtorNode (BIN_OP, op_code, left_value,
                                     right_value, nullptr, tree);

        left_value = new_node;

        is_bin_operation = IsBinOperation (tokens_array, *token_index);

        is_add = tokens_array [*token_index] .bin_op_code == ADD;

        is_sub = tokens_array [*token_index] .bin_op_code == SUB;
    }

    return left_value;
}

static BinTree_node*
GetTerm (GrammarParams)
{
    params_assert;

    BinTree_node* left_value  = GetPrimary (GiveParams);
    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    bool is_bin_operation = IsBinOperation (tokens_array, *token_index);

    if (!is_bin_operation) return left_value;

    bool is_mul = tokens_array [*token_index] .bin_op_code == MUL;

    bool is_div = tokens_array [*token_index] .bin_op_code == DIV;

    bool is_pow = tokens_array [*token_index] .bin_op_code == POW;

    while (is_bin_operation && (is_mul || is_div || is_pow))
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .bin_op_code;

        right_value = GetPrimary (GiveParams);

        new_node = BinTree_CtorNode (BIN_OP, op_code, left_value,
                                     right_value, nullptr, tree);

        left_value = new_node;

        is_bin_operation = IsBinOperation (tokens_array, *token_index);

        is_mul = tokens_array [*token_index] .bin_op_code == MUL;

        is_div = tokens_array [*token_index] .bin_op_code == DIV;

        is_pow = tokens_array [*token_index] .bin_op_code == POW;
    }

    return left_value;
}

static BinTree_node*
GetPrimary (GrammarParams)
{
    params_assert;

    BinTree_node* node = nullptr;

    if (IsPunctuation (tokens_array, *token_index) &&
        tokens_array [*token_index] .punct_op_code == OPEN_PARENTHESIS)
    {
        (*token_index)++;

        node = GetComparison (GiveParams);

        syn_assert (IsPunctuation (tokens_array, *token_index) &&
                    tokens_array [*token_index]
                    .punct_op_code == CLOSE_PARENTHESIS);

        (*token_index)++;

        return node;
    }

    return GetValue (GiveParams);
}

static BinTree_node*
GetValue (GrammarParams)
{
    params_assert;

    var_index_type func_index = VAR_INDEX_POISON;
    BinTree_node*  func_args  = nullptr;

    op_code_type   un_operation = OP_CODE_POISON;
    BinTree_node*  un_op_args   = nullptr;

    switch (tokens_array [*token_index] .token_data_type)
    {
        case NUMBER:
            return
                BinTree_CtorNode (NUMBER,
                                  tokens_array [(*token_index)++] .num_value,
                                  nullptr, nullptr, nullptr, tree);

        case VARIABLE:
            return GetVariable  (GiveParams);

        case BIN_OP:
            return GetOperation (GiveParams);

        case FUNCTION:
            func_index = GetFunctionIndex (GiveParams);
            (*token_index)++;
            func_args  = GetFunctionArgs  (GiveParams);

            return BinTree_CtorNode (FUNCTION, func_index, nullptr,
                                     func_args, nullptr, tree);

        case UN_OP:
            un_operation = tokens_array [*token_index] .un_op_code;
            (*token_index)++;
            un_op_args = GetExpression (GiveParams);

            return
                BinTree_CtorNode (UN_OP, un_operation, nullptr,
                                  un_op_args, nullptr, tree);

        case PUNCTUATION: [[fallthrough]];
        case KEY_OP:      [[fallthrough]];
        case NO_TYPE:     [[fallthrough]];

        default:
            syn_assert (0);
            return nullptr;
    }
}

static BinTree_node*
GetVariable (GrammarParams)
{
    params_assert;

    for (var_index_type i = 0;
                        i < tree -> name_table .var_table -> data_size; i++)
    {
        if (strcmp (tokens_array [*token_index] .var_name,
                    tree -> name_table .var_table -> data [i]) == 0)
        {
            (*token_index)++;
            return BinTree_CtorNode (VARIABLE, i, nullptr,
                                     nullptr, nullptr, tree);
        }
    }

    char* new_str = (char*) calloc (VAR_NAME_MAX_LEN, sizeof (char));
    if (!new_str)
    {
        perror ("new_str allocation error");
        return nullptr;
    }

    strcpy (new_str, tokens_array [(*token_index)++] .var_name);

    StackPush (tree -> name_table .var_table, new_str);

    return BinTree_CtorNode (VARIABLE, tree -> name_table
                            .var_table -> data_size - 1,
                             nullptr, nullptr, nullptr, tree);
}

static inline void
syn_assert_func (const size_t n_line,
                 const bool expression)
{
    if (!expression)
    {
        fprintf (stderr, "Aborting line %zd\n", n_line);
        abort ();
    }
}

static var_index_type
GetFunctionIndex (GrammarParams)
{
    params_assert;

    for (var_index_type i = 0;
                        i < tree -> name_table .func_table -> data_size; i++)
    {
        if (strcmp (tree -> name_table .func_table -> data [i],
                    tokens_array [*token_index] .var_name) == 0)
        {
            return i;
        }
    }

    return VAR_INDEX_POISON;
}

static inline bool
IsType (const List_data_type* const tokens_array,
        const size_t                token_index,
        const data_type             type)
{
    assert (tokens_array);

    if (tokens_array [token_index] .token_data_type == type)
        return true;

    return false;
}
