#ifndef LOX_H_
#define LOX_H_

#define MAX_LEXICAL_LEN 128

enum TokenType {
	LOX_TOKEN_EOF = 0,

	/* Single-charracter tokens. */
	LOX_LEFT_PAREN, LOX_RIGHT_PAREN, LOX_LEFT_BRACE,
	LOX_RIGHT_BRACE, LOX_COMMA, LOX_DOT,
	LOX_MINUS, LOX_PLUS, LOX_SEMICOLON,
	LOX_SLASH, LOX_STAR,

	/* One or two character tokens. */
	LOX_BANG, LOX_BANG_EQUAL, LOX_EQUAL,
	LOX_EQUAL_EQUAL, LOX_GREATER, LOX_GREATER_EQUAL,
	LOX_LESS, LOX_LESS_EQUAL,

	/* Literals. */
	LOX_IDENTIFIER, LOX_STRING, LOX_NUMBER,

	/* Keywords. */
	LOX_AND, LOX_CLASS, LOX_ELSE, LOX_FALSE,
	LOX_FUN, LOX_FOR, LOX_IF, LOX_NIL, LOX_OR,
	LOX_PRINT, LOX_RETURN, LOX_SUPER, LOX_THIS,
	LOX_TRUE, LOX_VAR, LOX_WHILE,
};

struct token {
	enum TokenType type;
	char lexeme[MAX_LEXICAL_LEN];
	void *literal;
	int line;
};

int run_lox_source_file(const char *path);

#endif /* LOX_H_*/
