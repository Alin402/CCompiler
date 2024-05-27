#pragma once

enum TokenTypes
{
	ID, // 0
	CT_INT, // 1
	CT_REAL, // 2
	CT_CHAR, // 3
	CT_STRING, // 4
	COMMA, // 5
	SEMICOLON, // 6
	LPAR, // 7
	RPAR, // 8
	LBRACKET, // 9
	RBRACKET, // 10
	LACC, // 11
	RACC, // 12
	ADD, // 13
	SUB, // 14
	MUL, // 15
	DIV, // 16
	DOT, // 17
	AND, // 18
	OR, // 19
	EQUAL, // 20
	ASSIGN, // 21
	NOTEQ, // 22
	NOT, // 23
	LESSEQ, // 24
	LESS, // 25
	GREATEREQ, // 26
	GREATER, // 27
	SPACE, // 28
	BREAK, // 29
	CHAR, // 30
	END, // 31
	DOUBLE, // 32
	ELSE, // 33
	FOR, // 34
	IF, // 35
	INT, // 36
	RETURN, // 37
	STRUCT, // 38
	VOID, // 39
	WHILE // 40
};

// For more conveniant debugging
char* token_names[] = {
	"identifier",
	"ct_int",
	"ct_real",
	"ct_char",
	"ct_string",
	"comma",
	"semicolon",
	"lpar",
	"rpar",
	"lbracket",
	"rbracket",
	"lacc",
	"racc",
	"add",
	"sub",
	"mul",
	"div",
	"dot",
	"end",
	"or",
	"equal",
	"assign",
	"noteq",
	"not",
	"lesseq",
	"less",
	"greatereq",
	"greater",
	"space",
	"break",
	"char",
	"end",
	"double",
	"else",
	"for",
	"if",
	"int",
	"return",
	"struct",
	"void",
	"while"
};

// Representation of token
typedef struct _Token
{
	int code;

	union
	{
		char* text;
		long int integer;
		double real;
		char character;
	};

	struct Token* next;
	int line;
} Token;

// Linked-list of tokens
Token* first, * last;

inline void add_new_token(int code, char* value, int line)
{
	Token* new_token = (Token*)malloc(sizeof(Token));
	if (!new_token)
	{
		printf("Problem allocating new token");
		return;
	}
	new_token->code = code;
	new_token->line = line;
	new_token->next = NULL;
	// Add either text, integer, real or character value depending on token
	if (code == CT_INT)
	{
		new_token->integer = atoi(value);
	}
	else if (code == CT_CHAR)
	{
		new_token->character = value[0];
	}
	else if (code == CT_REAL)
	{
		new_token->real = strtod(value, NULL);
	}
	else
	{
		new_token->text = value;
	}
	if (!first)
	{
		first = new_token;
		last = new_token;
	}
	else
	{
		last->next = new_token;
		last = new_token;
	}
}

inline void iterate_through_tokens(void)
{
	Token* p = first;
	while (p)
	{
		if (p->code == CT_INT)
		{
			printf("token: %s, value: %d, line: %d\n", token_names[p->code], p->integer, p->line);
		}
		else if (p->code == CT_REAL)
		{
			printf("token: %s, value: %f, line: %d\n", token_names[p->code], p->real, p->line);
		}
		else if (p->code == CT_CHAR)
		{
			printf("token: %s, value: %c, line: %d\n", token_names[p->code], p->character, p->line);
		}
		else
		{
			printf("token: %s, value: %s, line: %d\n", token_names[p->code], p->text, p->line);
		}
		p = p->next;
	}
	printf("\n");
}

inline char* allocate_one_character_in_buffer(char* s, int* buffer_size, char c)
{
	if (!buffer_size) {
		return NULL;
	}
	if (!s)
	{
		s = (char*)malloc(2 * sizeof(char));
		if (!s) {
			return NULL;
		}
		s[0] = c;
		s[1] = '\0';
		*buffer_size = 2;
	}
	else
	{
		char* temp = (char*)realloc(s, (*buffer_size + 1) * sizeof(char));
		if (!temp) {
			return NULL;
		}
		s = temp;
		s[*buffer_size - 1] = c;
		s[*buffer_size] = '\0';
		(*buffer_size)++;
	}
	return s;
}

// dynamically allocated token value
char* buffer;
int buffer_size;
int c;
int line = 1;
int state = 0;

inline void allocate_buffer_and_consume_character(FILE* fp)
{
	buffer = allocate_one_character_in_buffer(buffer, &buffer_size, c);
	c = fgetc(fp);
}

inline int check_if_delimiter_or_operator(char c)
{
	char* delimiters_and_operators = ",;()[]{}+-*/.&|!=!<>";
	if (!strchr(delimiters_and_operators, c) || c == EOF)
	{
		return 0;
	}
	return 1;
}

inline int parse_input_file(char* file_name)
{
	FILE* fp = fopen(file_name, "r");
	if (fp == NULL)
	{
		printf("File unable to open");
		return 0;
	}
	c = fgetc(fp);
	int end = 0;
	while (c && !end)
	{
		switch (state)
		{
		case 0:
			if (c == EOF)
			{
				end = 1;
				break;
			}
			else if (isalpha(c) || c == '_')
			{
				state = 1;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c >= '1' && c <= '9')
			{
				state = 3;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == '0')
			{
				state = 5;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == '\'')
			{
				state = 14;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == '"')
			{
				state = 19;
				allocate_buffer_and_consume_character(fp);
			}
			else if (isspace(c) && c != '\n')
			{
				while (isspace(c) && c != '\n')
				{
					c = fgetc(fp);
				}
			}
			else if (c == '\n')
			{
				line++;
				c = fgetc(fp);
			}
			else if (c == ',')
			{
				state = 24;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, ',');
				c = fgetc(fp);
			}
			else if (c == ';')
			{
				state = 25;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, ';');
				c = fgetc(fp);
			}
			else if (c == '(')
			{
				state = 26;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '(');
				c = fgetc(fp);
			}
			else if (c == ')')
			{
				state = 27;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, ')');
				c = fgetc(fp);
			}
			else if (c == '[')
			{
				state = 28;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '[');
				c = fgetc(fp);
			}
			else if (c == ']')
			{
				state = 29;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, ']');
				c = fgetc(fp);
			}
			else if (c == '{')
			{
				state = 30;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '{');
				c = fgetc(fp);
			}
			else if (c == '}')
			{
				state = 31;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '}');
				c = fgetc(fp);
			}
			else if (c == '+')
			{
				state = 32;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '+');
				c = fgetc(fp);
			}
			else if (c == '-')
			{
				state = 33;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '-');
				c = fgetc(fp);
			}
			else if (c == '*')
			{
				state = 34;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '*');
				c = fgetc(fp);
			}
			else if (c == '/')
			{
				char next_c = fgetc(fp);
				ungetc(next_c, fp);

				if (next_c == '/' || next_c == '*')
				{
					state = 54;
					c = fgetc(fp);
				}
				else
				{
					state = 35;
					buffer = NULL;
					buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '/');
					c = fgetc(fp);
				}
			}
			else if (c == '.')
			{
				state = 36;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '.');
				c = fgetc(fp);
			}
			else if (c == '&')
			{
				state = 37;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '&');
				c = fgetc(fp);
				}
			else if (c == '|')
			{
				state = 39;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '|');
				c = fgetc(fp);
				}
			else if (c == '=')
			{
				state = 41;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '=');
				c = fgetc(fp);
				}
			else if (c == '!')
			{
				state = 44;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '!');
				c = fgetc(fp);
				}
			else if (c == '<')
			{
				state = 47;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '<');
				c = fgetc(fp);
				}
			else if (c == '>')
			{
				state = 50;
				buffer = NULL;
				buffer = allocate_one_character_in_buffer(buffer, &buffer_size, '>');
				c = fgetc(fp);
			}
			else
			{
				printf("File not valid in state 0\n");
				return 0;
			}
			break;
		case 1:
			if (isalnum(c) || c == '_')
			{
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				if (isspace(c) || check_if_delimiter_or_operator(c))
				{
					state = 2;
				}
				else
				{
					printf("File not valid in state 1");
					return 0;
				}
			}
			break;
		case 2:
			if (strcmp(buffer, "int") == 0)
			{
				add_new_token(INT, buffer, line);
			}
			else if (strcmp(buffer, "char") == 0)
			{
				add_new_token(CHAR, buffer, line);
			}
			else if (strcmp(buffer, "double") == 0)
			{
				add_new_token(DOUBLE, buffer, line);
			}
			else if (strcmp(buffer, "else") == 0)
			{
				add_new_token(ELSE, buffer, line);
			}
			else if (strcmp(buffer, "for") == 0)
			{
				add_new_token(FOR, buffer, line);
			}
			else if (strcmp(buffer, "if") == 0)
			{
				add_new_token(IF, buffer, line);
			}
			else if (strcmp(buffer, "break") == 0)
			{
				add_new_token(BREAK, buffer, line);
			}
			else if (strcmp(buffer, "return") == 0)
			{
				add_new_token(RETURN, buffer, line);
			}
			else if (strcmp(buffer, "struct") == 0)
			{
				add_new_token(STRUCT, buffer, line);
			}
			else if (strcmp(buffer, "void") == 0)
			{
				add_new_token(VOID, buffer, line);
			}
			else if (strcmp(buffer, "while") == 0)
			{
				add_new_token(WHILE, buffer, line);
			}
			else
			{
				add_new_token(ID, buffer, line);
			}
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 3:
			if (c >= '0' && c <= '9')
			{
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == 'e' || c == 'E')
			{
				state = 10;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == '.')
			{
				state = 9;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				int k = check_if_delimiter_or_operator(c);

				if (isspace(c) || check_if_delimiter_or_operator(c))
				{
					state = 4;
				}
				else
				{
					printf("File not valid in state 3");
					return 0;
				}
			}
			break;
		case 4:
			add_new_token(CT_INT, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 5:
			if (c >= '0' && c <= '7')
			{
				state = 6;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == 'x')
			{
				state = 7;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == 'e' || c == 'E')
			{
				state = 10;
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == '.')
			{
				state = 9;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				if (isspace(c) || check_if_delimiter_or_operator(c))
				{
					state = 4;
				}
				else
				{
					printf("File not valid in state 5");
					return 0;
				}
			}
			break;
		case 6:
			if (c >= '0' && c <= '7')
			{
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				if (isspace(c) || check_if_delimiter_or_operator(c))
				{
					state = 4;
				}
				else
				{
					printf("File not valid in state 6");
					return 0;
				}
			}
			break;
		case 7:
			if (c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F')
			{
				state = 8;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 7");
				return 0;
			}
			break;
		case 8:
			if (c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F')
			{
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				if (isspace(c) || check_if_delimiter_or_operator(c))
				{
					state = 4;
				}
				else
				{
					printf("File not valid in state 8");
					return 0;
				}
			}
			break;
		case 9:
			if (c >= '0' && c <= '9')
			{
				state = 62;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 9");
				return 0;
			}
			break;
		case 62:
			if (c >= '0' && c <= '9')
			{
				allocate_buffer_and_consume_character(fp);
			}
			else if (c == 'e' || c == 'E')
			{
				state = 10;
				allocate_buffer_and_consume_character(fp);
			}
			else if (!isspace(c))
			{
				if (isspace(c) || check_if_delimiter_or_operator(c))
				{
					state = 13;
				}
				else
				{
					printf("File not valid in state 62");
					return 0;
				}
			}
			break;
		case 10:
			if (c == '+' || c == '-')
			{
				state = 11;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				state = 11;
			}
			break;
		case 11:
			if (c >= '0' && c <= '9')
			{
				state = 12;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 11");
				return 0;
			}
			break;
		case 12:
			if (c >= '0' && c <= '9')
			{
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				if (isspace(c) || check_if_delimiter_or_operator(c))
				{
					state = 13;
				}
				else
				{
					printf("File not valid in state 12");
					return 0;
				}
			}
			break;
		case 13:
			add_new_token(CT_REAL, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 14:
			if (c == '\\')
			{
				state = 16;
				allocate_buffer_and_consume_character(fp);
			}
			else if (strchr("\\'", c) == 0)
			{
				state = 17;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 14");
				return 0;
			}
			break;
		case 16:
			if (strchr("abfnrtv'?\"\\0", c))
			{
				state = 17;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 16");
				return 0;
			}
			break;
		case 17:
			if (c == '\'')
			{
				state = 18;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 17");
				return 0;
			}
			break;
		case 18:
			add_new_token(CT_CHAR, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 19:
			if (c == '\\')
			{
				state = 21;
				allocate_buffer_and_consume_character(fp);
			}
			else if (strchr("\\\"", c) == 0)
			{
				state = 22;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 19");
				return 0;
			}
			break;
		case 21:
			if (strchr("abfnrtv'?\"\\0", c))
			{
				state = 22;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 21");
				return 0;
			}
			break;
		case 22:
			if (c == '"')
			{
				state = 23;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				state = 19;
			}
			break;
		case 23:
			add_new_token(CT_STRING, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 24:
			add_new_token(COMMA, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 25:
			add_new_token(SEMICOLON, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 26:
			add_new_token(LPAR, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 27:
			add_new_token(RPAR, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 28:
			add_new_token(LBRACKET, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 29:
			add_new_token(RBRACKET, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 30:
			add_new_token(LACC, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 31:
			add_new_token(RACC, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 32:
			add_new_token(ADD, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 33:
			add_new_token(SUB, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 34:
			add_new_token(MUL, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 35:
			add_new_token(DIV, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 36:
			add_new_token(DOT, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 37:
			if (c == '&')
			{
				state = 38;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 37");
				return 0;
			}
			break;
		case 38:
			add_new_token(AND, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 39:
			if (c == '|')
			{
				state = 40;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				printf("File not valid in state 39");
				return 0;
			}
			break;
		case 40:
			add_new_token(OR, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 41:
			if (c == '=')
			{
				state = 42;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				state = 43;
			}
			break;
		case 42:
			add_new_token(EQUAL, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 43:
			add_new_token(ASSIGN, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 44:
			if (c == '=')
			{
				state = 45;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				state = 46;
			}
			break;
		case 45:
			add_new_token(NOTEQ, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 46:
			add_new_token(NOT, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 47:
			if (c == '=')
			{
				state = 48;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				state = 49;
			}
			break;
		case 48:
			add_new_token(LESSEQ, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 49:
			add_new_token(LESS, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 50:
			if (c == '=')
			{
				state = 51;
				allocate_buffer_and_consume_character(fp);
			}
			else
			{
				state = 52;
			}
			break;
		case 51:
			add_new_token(GREATEREQ, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 52:
			add_new_token(GREATER, buffer, line);
			buffer = NULL;
			buffer_size = 0;
			state = 0;
			break;
		case 54:
			if (c == '/')
			{
				state = 55;
				c = fgetc(fp);
			}
			else if (c == '*')
			{
				state = 57;
				c = fgetc(fp);
			}
			else
			{
				printf("File not valid in state 54");
				return 0;
			}
			break;
		case 55:
			if (c != '\n' && c != EOF)
			{
				c = fgetc(fp);
			}
			else
			{
				state = 0;
			}
			break;
		case 56:
			state = 0;
			break;
		case 57:
			if (c == '*')
			{
				state = 58;
			}
			else if (c != EOF)
			{
				state = 57;
			}
			c = fgetc(fp);
			break;

		case 58:
			if (c == '*')
			{
				state = 58;
			}
			else if (c == '/')
			{
				state = 0;
			}
			else
			{
				state = 57;
			}
			c = fgetc(fp);
			break;
		case 59:
			if (c == '*')
			{
				state = 60;
				c = fgetc(fp);
			}
			else
			{
				state = 57;
				c = fgetc(fp);
			}
			break;
		case 60:
			if (c == '*')
			{
				c = fgetc(fp);
			}
			else if (c == '/')
			{
				state = 61;
				c = fgetc(fp);
			}
			else
			{
				printf("File not valid in state 60");
				return 0;
			}
			break;
		case 61:
			state = 0;
			break;
		default:
			break;
		}
	}
	
	// add end token
	add_new_token(END, "end_token", line);
	return 1;
}

inline int run_lexical_analyzer(char *file_name)
{
	int ok = parse_input_file(file_name);
	if (ok)
	{
		printf("Lexical analysis passed\n\n");
		iterate_through_tokens();
	}
	free(buffer);
	return 0;
}