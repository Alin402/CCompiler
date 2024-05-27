#pragma once
#include "LexicalAnalyzer.h"

inline void test_all(int n)
{
    // for (int i = 0; i < n; i++)
    // {
    //     char file_number = i + '0';
    //     char str[100];
    //     str[0] = file_number;
    //     str[1] = '\0';
    //     strcat_s(str, sizeof(str), ".c");
    //     char file_name[100] = "Tests/";
    //     strcat_s(file_name, sizeof(file_name), str);
    //     
    //     printf("Testing begins for file %s\n\n", file_name);
    //     
    //     RunLexicalAnalyzer(file_name);
    //     RunSyntacticAnalyzer();
    //     
    //     printf("Testing succeeded for file %s\n\n", file_name);
    // }

    run_lexical_analyzer("Tests/9.c");
    run_syntactic_analyzer();
}
