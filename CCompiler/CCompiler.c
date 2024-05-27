#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "LexicalAnalyzer.h"
#include "Errors.h"
#include "SemanticAnalyzer.h"
#include "SyntacticAnalyzer.h"
#include "Testing.h";

int main(void)
{
    run_lexical_analyzer("Tests/5.c");
    initialize_semantic_analyzer();
    run_syntactic_analyzer();
    free_symbols_table();
    return 0;
}
