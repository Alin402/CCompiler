#pragma once
#include "SemanticAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "SemanticAnalyzer.h"

Token *consumed_token;
Token *current_token = NULL;

int unit(void);
int decl_struct(void);
int decl_var(void);
int type_base(Type *type);
int array_decl(Type *type);
int type_name(Type *type);
int decl_func(void);
int func_arg(void);
int stm(void);
int stm_compound(void);
int expr(void);
int expr_assign(void);
int expr_or(void);
int expr_or1(void);
int expr_and(void);
int expr_and1(void);
int expr_eq(void);
int expr_eq1(void);
int expr_rel(void);
int expr_rel1(void);
int expr_add(void);
int expr_add1(void);
int expr_mul(void);
int expr_mul1(void);
int expr_cast(void);
int expr_unary(void);
int expr_postfix(void);
int expr_postfix1(void);
int expr_primary(void);

void addStruct(Token *tkName);
void clearStruct();

inline int consume(const int code)
{
	if (current_token->code == code)
	{
		consumed_token = current_token;
		current_token = current_token->next;
		return 1;
	}
	return 0;
}

inline int unit(void)
{
	while (1)
	{
		if (decl_struct()) {}
		else if (decl_func()) {}
		else if (decl_var()) {}
		else break;
	}
	if (!consume(END)) tkerr(current_token, "Missing <<end>> of file.");
	return 1;
}

inline int decl_struct(void)
{
	Token *start_token = current_token;
	if (!consume(STRUCT)) return 0;
	if (!consume(ID)) tkerr(current_token, "Missing <<identifier>> after <<struct>> declaration.");
	// ST
	Token *token_name = consumed_token;
	// EST
	if (!consume(LACC))
	{
		current_token = start_token;
		return 0;
	}
	// ST
	if(find_symbol(symbols, token_name->text))
	{
		tkerr(current_token,"symbol redefinition: %s", token_name->text);
	}
	current_struct=add_symbol(symbols, token_name->text, CLS_STRUCT);
	init_symbols(&current_struct->members);
	// EST
	while (1)
	{
		if (!decl_var()) {}
		else break;
	}
	if (!consume(RACC)) tkerr(current_token, "Missing <<}>> after <<struct>> declaration.");
	if (!consume(SEMICOLON)) tkerr(current_token, "Missing <<;>> after <<struct>> declaration.");
	// ST
	current_struct = NULL;
	// EST
	return 1;
}

inline int decl_var(void)
{
	Token *start_token = current_token;
	// ST
	Type *type = (Type*)malloc(sizeof(Type));
	//EST
	if (!type_base(type)) return 0;
	if (!consume(ID))
	{
		current_token = start_token;
		return 0;	
	}
	// ST
	Token *token_name = consumed_token;
	//EST
	if (!array_decl(type))
	{
		// ST
		type->noElements = -1;
		// EST
	}
	// ST
	add_var(token_name, type);
	// EST
	while (1)
	{
		if (!consume(COMMA)) break;
		if (!consume(ID)) tkerr(current_token, "Missing <<identifier>> after <<,>> in VAR declaration.");
		// ST
		token_name = consumed_token;
		// EST
		if (!array_decl(type))
		{
			// ST
			type->noElements = -1;
			// EST
		}
		// ST
		add_var(token_name, type);
		// EST
	}
	if (consume(SEMICOLON)) return 1;
	else
	{
		tkerr(current_token, "Missing <<;>> after <<variable>> declaration in <<struct>>.");	
	}
	current_token = start_token;
	return 0;
}

inline int type_base(Type *type)
{
	Token *start_token = current_token;
	if (consume(INT))
	{
		// ST
		type->typeBase = TB_INT;
		// EST
		return 1;	
	}
	if (consume(DOUBLE))
	{
		// ST
		type->typeBase = TB_DOUBLE;
		// EST
		return 1;	
	}
	if (consume(CHAR))
	{
		// ST
		type->typeBase = TB_CHAR;
		// EST
		return 1;
	}
	if (consume(STRUCT))
	{
		// ST
		Token *token_name = NULL;
		// EST
		if (consume(ID))
		{
			// ST
			token_name = consumed_token;
			Symbol *s = find_symbol(symbols, token_name->text);
			if (s == NULL) tkerr(current_token, "undefined symbol: %s", token_name->text);
			if(s->cls != CLS_STRUCT) tkerr(current_token,"%s is not a struct", token_name->text);
			type->typeBase=TB_STRUCT;
			type->s=s;
			// EST
			return 1;	
		}
		else tkerr(current_token, "Missing <<identifier>> after <<type base>>.");
	}
	current_token = start_token;
	return 0;
}

inline int array_decl(Type *type)
{
	if (!consume(LBRACKET)) return 0;
	if (expr())
	{
		// ST
		type->noElements = 0;
		// EST
	}
	if (!consume(RBRACKET)) tkerr(current_token, "Missing <<]>> after <<array>> declaration.");
	return 1;
}

inline int type_name(Type *type)
{
	if (type_base(type))
	{
		if (!array_decl(type))
		{
			type->noElements = -1;
		}
		return 1;
	}
	return 0;
}

inline int decl_func_1(Type *type)
{
	Token *start_token = current_token;
	if (!consume(ID)) return 0;
	Token *token_name = consumed_token;
		if (consume(LPAR))
		{
			// ST
			if(find_symbol(symbols, token_name->text))
			{
				tkerr(current_token,"symbol redefinition: %s", token_name->text);	
			}
			current_func = add_symbol(symbols, token_name->text, CLS_FUNC);
			init_symbols(&current_func->args);
			current_func->type = *type;
			current_depth++;
			// EST
			if (func_arg())
			{
				while (1)
				{
					if (consume(COMMA))
					{
						if (func_arg())
						{
							continue;
						}
						else tkerr(current_token, "Missing <<argument list>> in <<function>> declaration.");
					}
					else break;
				}
			}
			if (consume(RPAR))
			{
				// ST
				current_depth--;
				// EST
				if (stm_compound())
				{
					// ST
					delete_symbols_after(symbols, current_func);
					current_func = NULL;
					// EST
					return 1;
				}
				else tkerr(current_token, "Missing <<stm_compound>> in <<function>> declaration.");
			}
			else tkerr(current_token, "Missing <<(>> in function declaration.");
		}
	current_token = start_token;
	return 0;
}

inline int decl_func(void)
{
	Token *start_token = current_token;
	// ST
	Type *type = (Type*)malloc(sizeof(Type));
	// EST
	if (type_base(type))
	{
		if (consume(MUL))
		{
			// ST
			type->noElements = 0;
			// EST
		}
		else
		{
			// ST
			type->noElements = -1;
			// EST
		}
		if (!decl_func_1(type))
		{
			current_token = start_token;
			return 0;
		}
		return 1;
	}
	if (consume(VOID))
	{
		// ST
		type->typeBase = TB_VOID;
		// EST
		if (!decl_func_1(type))
			tkerr(current_token, "Missing <<identifier>> after <<function void>> declaration.");
		return 1;
	}
	return 0;
}

inline int func_arg(void) {
	Token *start_token = current_token;
	// ST
	Type *type = (Type*)malloc(sizeof(Type));
	Token *token_name = NULL;
	// EST
	if (type_base(type))
	{
		if (consume(ID))
		{
			token_name = consumed_token;
			if (!array_decl(type))
			{
				// ST
				type->noElements = -1;
				// EST
			}
			// ST
			Symbol  *s = add_symbol(symbols, token_name->text, CLS_VAR);
			s->mem=MEM_ARG;
			s->type = *type;
			s = add_symbol(&current_func->args, token_name->text, CLS_VAR);
			s->mem = MEM_ARG;
			s->type = *type;
			// EST
			return 1;
		}
		else tkerr(current_token, "Missing <<identifier>> for argument in function argument list.");
	}
	current_token = start_token;
	return 0;
}

inline int stm(void)
{
	Token *start_token = current_token;
	if (stm_compound()) return 1;
	if (consume(IF))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (stm())
					{
						if (consume(ELSE))
						{
							if (stm()) return 1;
							else tkerr(current_token, "Missing <<stm>> after <<else>>.");
						}
						return 1;
					}
					else tkerr(current_token, "Missing <<stm>> in <<if>>.");
				}
				else tkerr(current_token, "Missing <<)>> in <<if>>.");
			}
			else tkerr(current_token, "Missing <<expression>> in <<if>>.");
		}
		else tkerr(current_token, "Missing <<(>> in <<if>>.");
	}
	current_token = start_token;
	if (consume(WHILE))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (stm())
					{
						return 1;
					}
					else tkerr(current_token, "Missing <<stm>> in <<while>>.");
				}
				else tkerr(current_token, "Missing <<)>> in <<while>>.");
			}
			else tkerr(current_token, "Missing <<expression>> in <<while>>.");
		}
		else tkerr(current_token, "Missing <<(>> in <<while>>.");
	}
	current_token = start_token;
	if (consume(FOR))
	{
		if (!consume(LPAR)) tkerr(current_token, "Missing <<(>> in for.");
		expr();
		if (!consume(SEMICOLON)) tkerr(current_token, "Missing <<;>> in for.");
		expr();
		if (!consume(SEMICOLON)) tkerr(current_token, "Missing <<;>> in for.");
		expr();
		if (!consume(RPAR)) tkerr(current_token, "Missing <<)>> in for.");
		if (!stm()) tkerr(current_token, "Missing <<stm>> in for.");
		return 1;
	}
	current_token = start_token;
	if (consume(BREAK))
	{
		if (consume(SEMICOLON))
			return 1;
		else tkerr(current_token, "Missing <<;>> for <<break>> statement.");
	}
	current_token = start_token;
	if (consume(RETURN))
	{
		expr();
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else tkerr(current_token, "Missing <<;>> for return statement.");
	}
	current_token = start_token;
	if (consume(SEMICOLON))
	{
		return 1;
	}
	else
	{
		if (expr())
		{
			if (!consume(SEMICOLON)) tkerr(current_token, "Missing <<;>> after expression.");
			return 1;
		}
	}
	current_token = start_token;
	return 0;
}

inline int stm_compound(void)
{
	Token *start_token = current_token;
	// ST
	Symbol *start = symbols->end[-1];
	// EST
	if (consume(LACC))
	{
		// ST
		current_depth++;
		// EST
		while (1)
		{
			if (decl_var()) continue;
			if (stm()) continue;
			else break;
		}
		if (consume(RACC))
		{
			current_depth--;
			delete_symbols_after(symbols, start);
			return 1;	
		}
		else tkerr(current_token, "Missing <<}>> declaration.");
	}
	current_token = start_token;
	return 0;
}

inline int expr(void)
{
	if (expr_assign())
		return 1;
	return 0;
}

inline int expr_assign(void)
{
	Token *start_token = current_token;
	if (consume(NOT) || consume(SUB))
	{
		if (expr_unary())
		{
			if (consume(ASSIGN))
			{
				if (expr_assign()) return 1;
				else tkerr(current_token, "Missing <<assign expression>> declaration.");
			}
		}
	}
	if (expr_postfix())
	{
		if (consume(ASSIGN))
		{
			if (expr_assign()) return 1;
			else tkerr(current_token,"Missing <<assign expression>>.");
		}
	}
	current_token = start_token;
	if (expr_or()) return 1;
	current_token = start_token;
	return 0;
}

inline int expr_or1(void)
{
	Token *start_token = current_token;
	if (consume(OR))
	{
		if (expr_and())
		{
			if (expr_or1()) return 1;
			else tkerr(current_token, "Incomplete <<or>> expression.");
		}
		else tkerr(current_token, "Missing <<and>> expression in <<or>> expression.");
	}
	current_token = start_token;
	return 1;
}

inline int expr_or(void)
{
	Token *start_token = current_token;
	if (expr_and())
	{
		if (expr_or1()) return 1;
		else tkerr(current_token, "Incomplete <<or>> expression.");
	}
	return 0;
}

inline int expr_and1(void)
{
	Token *start_token = current_token;
	if (consume(AND))
	{
		if (expr_eq())
		{
			if (expr_and1()) return 1;
			else tkerr(current_token, "Incomplete <<and>> expession.");
		}
		else tkerr(current_token, "Missing <<and>> in <<and>> expression.");
	}
	current_token = start_token;
	return 1;
}

inline int expr_and(void)
{
	if (expr_eq())
	{
		if (expr_and1()) return 1;
		else tkerr(current_token, "Incomplete <<and>> expression.");
	}
	return 0;
}

inline int expr_eq1(void)
{
	Token *start_token = current_token;
	if (consume(EQUAL))
	{
		if (expr_rel())
		{
			if (expr_eq1()) return 1;
			else tkerr(current_token, "Missing <<expression>> in <<expression>>");
		}
		else tkerr(current_token, "Missing <<rel>> expression.");
	}
	if (consume(NOTEQ))
	{
		if (expr_rel())
		{
			if (expr_eq1()) return 1;
			else tkerr(current_token, "Missing <<expression>> in <<expression>>");
			return 1;
		}
		else tkerr(current_token, "Missing <<rel>> expression.");
	}
	current_token = start_token;
	return 1;
}

inline int expr_eq(void)
{
	if (expr_rel())
	{
		if (expr_eq1()) return 1;
		else tkerr(current_token, "Incomplete <<equal>> expression.");
	}
	return 0;
}

inline int expr_rel_2(void)
{
	if (consume(LESS)) return 1;
	if (consume(LESSEQ)) return 1;
	if (consume(GREATER)) return 1;
	if (consume(GREATEREQ)) return 1;
	return 0;
}

inline int expr_rel1(void)
{
	Token *start_token = current_token;
	if (expr_rel_2())
	{
		if (expr_add())
		{
			if (expr_rel1()) return 1;
			else tkerr(current_token, "Missing <<rel>> expression");
		}
		else tkerr(current_token, "Missing <<add>> expression in <<rel>>.");
	}
	current_token = start_token;
	return 1;
}

inline int expr_rel(void)
{
	if (!expr_add()) return 0;
	if (!expr_rel1()) tkerr(current_token, "Incomplete <<rel>> expression.");
	return 1;
}

inline int expr_add1(void)
{
	Token *start_token = current_token;
	if (consume(ADD))
	{
		if (expr_add())
		{
			if (expr_add1()) return 1;
			else tkerr(current_token, "Missing <<expression>> in <<add>> expression.");
		}
		else tkerr(current_token, "Missing <<rel>> expression.");
	}
	if (consume(SUB))
	{
		if (expr_add())
		{
			if (expr_add1()) return 1;
			else tkerr(current_token, "Missing <<expression>> after <<sub>>.");
		}
		else tkerr(current_token, "Missing <<rel>> expression in <<add>> expression.");
	}
	current_token = start_token;
	return 1;
}

inline int expr_add(void)
{
	if (expr_mul()) {
		if (expr_add1()) return 1;
		else tkerr(current_token, "Incomplete <<add>> expression.");
	}
	return 0;
}

inline int expr_mul1(void)
{
	Token *start_token = current_token;
	if (consume(MUL))
	{
		if (expr_cast())
		{
			if (expr_mul1()) return 1;
			else tkerr(current_token, "Missing <<mul>> after <<cast>>.");
		}
		else tkerr(current_token, "Missing <<rel>> expression.");
	}
	if (consume(DIV))
	{
		if (expr_cast())
		{
			if (expr_mul1()) return 1;
			else tkerr(current_token, "Missing <<mul>> after cast.");
		}
		else tkerr(current_token, "Missing <<rel>> expression.");
	}
	current_token = start_token;
	return 1;
}

inline int expr_mul(void)
{
	Token *start_token = current_token;
	if (expr_cast())
	{
		if (expr_mul1()) return 1;
		else tkerr(current_token, "Incomplete <<mul>> expression.");
	}
	return 0;
}

inline int expr_cast(void)
{
	Token *start_token = current_token;
	// ST
	Type *type = (Type*)malloc(sizeof(Type));
	// EST
	if (consume(LPAR))
	{
		if (type_name(type))
		{
			if (consume(RPAR))
			{
				if (expr_cast())
				{
					return 1;
				}
				else tkerr(current_token, "Incomplete <<cast>> declaration.");
			}
			else tkerr(current_token, "Missing <<)>> expression after cast.");
		}
		else tkerr(current_token, "Missing <<type name>> after <<(>>.");
	}
	if (expr_unary()) {
		return 1;
	}
	current_token = start_token;
	return 0;
}

inline int expr_unary(void)
{
	Token *start_token = current_token;
	if (consume(SUB))
	{
		if (expr_unary())
			return 1;
		else tkerr(current_token, "Missing <<unary>> expression.");
	}
	if (consume(NOT))
	{
		if (expr_unary())
			return 1;
		else tkerr(current_token, "Missing <<unary>> expression.");
	}
	current_token = start_token;
	if (expr_postfix())
	{
		return 1;
	}
	current_token = start_token;
	return 0;
}

inline int expr_postfix1(void)
{
	Token *start_token = current_token;
	if (consume(LBRACKET))
	{
		if (expr())
		{
			if (consume(RBRACKET)) {
				if (expr_postfix1()) return 1;
				else tkerr(current_token, "Incomplete <<expression>> declaration.");
			}
			else tkerr(current_token, "Missing <<]>> after expression.");
		}
		else tkerr(current_token, "Missing <<expression>> after <<[>>.");
	}
	if (consume(DOT))
	{
		if (consume(ID))
		{
			if (expr_postfix1()) return 1;
			else tkerr(current_token, "Incomplete <<expression>>.");
		}
		else tkerr(current_token, "Missing <<identifier>> in <<postfix expression>>.");
	}
	current_token = start_token;
	return 1;
}

inline int expr_postfix(void)
{
	Token *start_token = current_token;
	if (expr_primary())
	{
		if (expr_postfix1()) return 1;
		else tkerr(current_token, "Incomplete <<postfix>> declaration.");
	}
	current_token = start_token;
	return 0;
}

inline int expr_primary(void)
{
	Token *start_token = current_token;
	if (consume(ID))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				while (1)
				{
					if (consume(COMMA))
					{
						if (expr()) continue;
						else tkerr(current_token, "Missing <<expression>> after <<,>>.");
					}
					break;
				}
			}
			if (consume(RPAR))
			{
				return 1;
			}
			else tkerr(current_token, "Missing <<)>> in <<primary expression>>.");
		}
		return 1;
	}
	if (consume(CT_INT)) return 1;
	if (consume(CT_REAL)) return 1;
	if (consume(CT_CHAR)) return 1;
	if (consume(CT_STRING)) return 1;
	if (consume(LPAR))
	{
		if (expr())
		{
			if (consume(RPAR))return 1;
			else tkerr(current_token, "Missing <<)>> in <<primary expression>>.");
		}
		else tkerr(current_token, "Missing expression after <<(>>.");
	}
	current_token = start_token;
	return 0;
}

inline int start(void)
{
	current_token = first;
	return unit();
}

inline void run_syntactic_analyzer(void)
{
	const int ok = start();
	if (ok)
	{
		printf("Syntactic analysis passed\n\n");
		iterate_through_symbols(symbols);
	}
	else
	{
		printf("Syntactic analysis did not pass\n\n");
	}
}