%{
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include "node.hpp"

std::vector<std::shared_ptr<NFunctionDeclaration>> programBlocks; /* the top level root node of our final AST */

extern "C" int yylex(void);
// char yytext[];
extern int column;

void yyerror(const char* s)
{
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);
}
%}
%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NFunctionDeclaration *func_decl;
    NVariableDeclaration *var_decl;
    VariableList *var_list;
    ExpressionList *exprvec;
    std::string *string;
    int token;
    int pointer_level;
}

%token <string> IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token <token> PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token <token> AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token <token> SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token <token> XOR_ASSIGN OR_ASSIGN TYPE_NAME
%token <token> NEG_OP ADD_OP MUL_OP DIV_OP MOD_OP LT_OP GT_OP
%token <token> CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <expr> expression assignment_expression conditional_expression  postfix_expression
%type <expr> primary_expression  equality_expression
%type <expr> additive_expression multiplicative_expression 
%type <expr> shift_expression relational_expression and_expression 
%type <expr> inclusive_or_expression exclusive_or_expression
%type <expr> logical_and_expression logical_or_expression
%type <var_decl> declaration
%type <ident> declarator unary_expression cast_expression
%type <var_list> function_parameters_list
/*%type <exprvec> call_args */
%type <func_decl> translation_unit function_definition
%type <stmt> statement declaration_statement expression_statement selection_statement iteration_statement jump_statement
%type <token> type_specifier
%type <pointer_level> pointer
%type <block> statement_list compound_statement

%start translation_unit
%%

primary_expression
	: IDENTIFIER {
		$$ = new NIdentifier(std::shared_ptr<std::string>(name), 0);
	}
	/* | CONSTANT
	| STRING_LITERAL
	| '(' expression ')' */
	;

postfix_expression
	: primary_expression
	/* | postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP */
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	/* | INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')' */
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	/* | '(' type_name ')' cast_expression */
	;

multiplicative_expression
	: multiplicative_expression MUL_OP cast_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| multiplicative_expression DIV_OP cast_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| multiplicative_expression MOD_OP cast_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	/* | cast_expression */
	;

additive_expression
	: multiplicative_expression
	| additive_expression ADD_OP multiplicative_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| additive_expression NEG_OP multiplicative_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| shift_expression RIGHT_OP additive_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	;

relational_expression
	: shift_expression
	| relational_expression LT_OP shift_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| relational_expression GT_OP shift_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| relational_expression LE_OP shift_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| relational_expression GE_OP shift_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| equality_expression NE_OP relational_expression {
		$$ = new NExpression(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	;

and_expression
	: equality_expression
	/* | and_expression '&' equality_expression */
	;

exclusive_or_expression
	: and_expression
	/* | exclusive_or_expression '^' and_expression */
	;

inclusive_or_expression
	: exclusive_or_expression
	/* | inclusive_or_expression '|' exclusive_or_expression */
	;

logical_and_expression
	: inclusive_or_expression
	/* | logical_and_expression AND_OP inclusive_or_expression */
	;

logical_or_expression
	: logical_and_expression
	/* | logical_or_expression OR_OP logical_and_expression */
	;

conditional_expression
	: logical_or_expression
	/* | logical_or_expression '?' expression ':' conditional_expression */
	;

assignment_expression
	: unary_expression assignment_operator assignment_expression {
		$$ = new NAssignment(std::shared_ptr<NIdentifier>($1), std::shared_ptr<NExpression>($3));
	}
	| conditional_expression
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	/* | expression ',' assignment_expression */
	;

constant_expression
	: conditional_expression
	;

declaration
	: type_specifier {
        $$ = new NVariableDeclaration($1, std::shared_ptr<NIdentifier>(nullptr));
    }
	| type_specifier declarator {
        $$ = new NVariableDeclaration($1, std::shared_ptr<NIdentifier>($2));
    }
	;

/* init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	; */

init_declarator
	: declarator
	/* | declarator '=' initializer */
	;

type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| TYPE_NAME
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

declarator
	: pointer direct_declarator { $$ = new NIdentifier(std::shared_ptr<std::string>($2), $1); }
	| direct_declarator { $$ = new NIdentifier(std::shared_ptr<std::string>($1), 0); }
	;

direct_declarator
	: IDENTIFIER
	/* | '(' declarator ')' */
	| direct_declarator '[' constant_expression ']'
	/* | direct_declarator '[' ']' */
	| direct_declarator '(' parameter_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
	;

pointer
	: '*' { $$ = 0; }
	| '*' pointer { $$ = $2 + 1; }
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: type_specifier declarator
	| type_specifier abstract_declarator
	| type_specifier

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	/* | '(' parameter_list ')' */
	| direct_abstract_declarator '(' ')'
	/* | direct_abstract_declarator '(' parameter_list ')' */
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: expression_statement {}
    | declaration_statement {}
	/* | selection_statement */
	/* | iteration_statement */
	/* | jump_statement */
	;

compound_statement
	: '{' '}' { $$ = new NBlock(); }
	| '{' statement_list '}' { $$ = $2; }
	;

declaration_list
	: declaration
	| declaration_list ',' declaration
	;

statement_list
	: statement {
        $$ = new NBlock();
        $$->statements->push_back(std::shared_ptr<NStatement>($1));
    }
	| statement_list statement {
        $1->statements->push_back(std::shared_ptr<NStatement>($2));
    }
	;

declaration_statement
    : declaration_list ';'
    ;

expression_statement
	: ';' { $$ = new NExpressionStatement(std::shared_ptr<NExpression>(nullptr)); }
	| expression ';' { $$ = new NExpressionStatement(std::shared_ptr<NExpression>($1)); }
	;

selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: function_definition { programBlocks.push_back(std::shared_ptr<NFunctionDeclaration>($1)); }
	| translation_unit function_definition { programBlocks.push_back(std::shared_ptr<NFunctionDeclaration>($2)); }
	;

/* function_parameters_list
	: declaration { $$ = new VariableList(); $$->push_back(std::shared_ptr<NVariableDeclaration>($1)); }
	| function_parameters_list ',' declaration { $1->push_back(std::shared_ptr<NVariableDeclaration>($3)); }
	; */

function_definition
	: type_specifier declarator compound_statement {
        $$ = new NFunctionDeclaration($1, std::shared_ptr<NIdentifier>($2),
        std::shared_ptr<VariableList>($4),
        std::shared_ptr<NBlock>($6));
    }
	;
%%
