#pragma once

Token *current_token;

enum { TB_INT, TB_DOUBLE, TB_CHAR, TB_STRUCT, TB_VOID };

typedef struct Symbol Symbol;
typedef struct Type Type;
typedef struct Symbols Symbols;

struct Type
{
    int typeBase;
    Symbol *s;
    int noElements;
};

enum { CLS_VAR, CLS_FUNC, CLS_EXTFUNC, CLS_STRUCT };
enum { MEM_GLOBAL, MEM_ARG, MEM_LOCAL };

struct Symbols
{
    Symbol **begin;
    Symbol **end;
    Symbol **after;
};

struct Symbol
{
    const char *name;
    int cls;
    int mem;
    Type type;
    int depth;
    union
    {
        Symbols args;
        Symbols members;
    };
};

inline void init_symbols(Symbols *symbols)
{
    symbols->begin = NULL;
    symbols->end = NULL;
    symbols->after = NULL;
}

// Global variables
Symbols *symbols;
Symbol *current_struct = NULL;
Symbol *current_func = NULL;
Symbol *current_var = NULL;
int current_depth = 0;

inline Symbol *add_symbol(Symbols *symbols, const char* name, int cls)
{
    Symbol *s;
    
    if (symbols->end == symbols->after)
    {
        int count = symbols->after - symbols->begin;
        int n = count * 2;
        if (n == 0)
        {
            n = 1;
        }
        symbols->begin = (Symbol**)realloc(symbols->begin, n * sizeof(Symbol*));
        if (symbols->begin == NULL)
        {
            err("Not enough memory...");
        }
        symbols->end = symbols->begin + count;
        symbols->after = symbols->begin + n;
    }
    s = (Symbol*)malloc(sizeof(Symbol));
    if (s == NULL)
    {
        err("Not enough memory...");
    }
    *symbols->end++ = s;
    s->name = name;
    s->cls = cls;
    s->depth = current_depth;
    
    return s;
}

inline Symbol* find_symbol(Symbols *symbols, const char *name)
{
    if (symbols == NULL || symbols->begin == NULL)
    {
        return NULL;
    }
    for (Symbol **s = symbols->end - 1; s >= symbols->begin; s--)
    {
        if (strcmp((*s)->name, name) == 0)
        {
            return *s;
        }
    }
    return NULL;
}

inline void add_var(Token *token_name, Type *t)
{
    Symbol *s;
    if(current_struct){
        if(find_symbol(&current_struct->members, token_name->text))
        {
            tkerr(current_token,"symbol redefinition: %s", token_name->text);
        }
        s = add_symbol(&current_struct->members,token_name->text,CLS_VAR);
    }
    else if(current_func){
        s = find_symbol(symbols,token_name->text);
        if(s && s->depth == current_depth)
        {
            tkerr(current_token, "symbol redefinition: %s", token_name->text);
        }
        s = add_symbol(symbols, token_name->text, CLS_VAR);
        s -> mem=MEM_LOCAL;
    }
    else{
        if(find_symbol(symbols, token_name->text))
        {
            tkerr(current_token,"symbol redefinition: %s",token_name->text);
        }
        s = add_symbol(symbols,token_name->text,CLS_VAR);
        s->mem = MEM_GLOBAL;
    }
    s->type = *t;
}

inline void delete_symbols_after(Symbols *symbols, Symbol *s)
{
    if (symbols == NULL || symbols->begin == NULL || s == NULL)
    {
        return;
    }
    Symbol **position = NULL;
    for (Symbol **sym = symbols->begin; sym < symbols->end; sym++)
    {
        if (*sym == s)
        {
            position = sym + 1;
            break;
        }
    }
    if (position == NULL)
    {
        return;
    }
    for (Symbol **sym = position; sym < symbols->end; sym++)
    {
        free(*sym);
    }
    symbols->end = position;
}

// Life cycle functions
inline void iterate_through_symbols(Symbols *symbols)
{
    if (symbols == NULL || symbols->begin == NULL)
    {
        printf("No symbols...\n");
    }
    else
    {
        for (Symbol **s = symbols->begin; s < symbols->end; s++)
        {
            printf("Symbol: %s, cls: %d, depth: %d, type: %d\n", (*s)->name, (*s)->cls, (*s)->depth, (*s)->type.typeBase);
        }
    }    
}

inline void initialize_semantic_analyzer(void)
{
    symbols = (Symbols*)malloc(sizeof(Symbols));
    if (symbols == NULL)
    {
        err("Not enough memory...");
    }
    init_symbols(symbols);
}

inline void free_symbols_table(void)
{
    for (Symbol **s = symbols->begin; s < symbols->end; s++)
    {
        free(*s);
    }
    free(symbols->begin);
    free(symbols);
}