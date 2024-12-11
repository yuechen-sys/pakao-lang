%{
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include "node.hpp"

std::vector<std::shared_ptr<NFunction>> programBlocks; /* the top level root node of our final AST */

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
	NFunction *func;
    NVariableDeclaration *var_decl;
    NVariableDeclarationList *var_list;
    ExpressionList *expr_list;
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
%type <expr> logical_and_expression logical_or_expression unary_expression cast_expression
%type <expr_list> argument_expression_list
%type <var_decl> parameter_declaration init_declarator
/* %type <var_decl> parameter_declaration */
%type <ident> declarator   direct_declarator
%type <var_list> parameter_list declaration init_declarator_list
// %type <exprvec> 
%type <func> translation_unit function_definition
%type <func_decl> func_declarator
%type <stmt> statement declaration_statement expression_statement selection_statement iteration_statement jump_statement
%type <token> type_specifier
%type <pointer_level> pointer
%type <block> statement_list compound_statement

%start translation_unit
%%

primary_expression
	: IDENTIFIER {
		$$ = new NIdentifier(std::shared_ptr<std::string>($1));
	}
	| CONSTANT {
        $$ = new NInteger(std::stoll(*$1));
		delete $1;
	}
    | '(' expression ')' {
        $$ = $2;
    }
	| STRING_LITERAL {
        $$ = new NStringLiteral(
            std::shared_ptr<std::string>($1)
        );
    }
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']' {
        $$ = new NArrayIndex(
            std::shared_ptr<NExpression>($1),
            std::shared_ptr<NExpression>($3)
        );
    }
	| IDENTIFIER '(' ')' {
        $$ = new NMethodCall(
            std::make_shared<NIdentifier>(
                std::shared_ptr<std::string>($1)
            ) 
        );
    }
	| IDENTIFIER '(' argument_expression_list ')' {
        $$ = new NMethodCall(
            std::make_shared<NIdentifier>(
                std::shared_ptr<std::string>($1)
            ),
            std::shared_ptr<ExpressionList>($3)
        );
    }
    /*
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
    */
	| postfix_expression INC_OP {
        $$ = new NUnaryOperator(
            std::shared_ptr<NExpression>($1),
            $2
        );
    }
	| postfix_expression DEC_OP {
        $$ = new NUnaryOperator(
            std::shared_ptr<NExpression>($1),
            $2
        );
    }
	;

argument_expression_list
	: assignment_expression {
		$$ = new ExpressionList();
		$$->push_back(std::shared_ptr<NExpression>($1));
    }
	| argument_expression_list ',' assignment_expression {
		$1->push_back(std::shared_ptr<NExpression>($3));
    }
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression {
        $$ = new NUnaryOperator(
            std::shared_ptr<NExpression>($2),
            $1
        );
    }
	| DEC_OP unary_expression {
        $$ = new NUnaryOperator(
            std::shared_ptr<NExpression>($2),
            $1
        );
    }
	/* | unary_operator cast_expression {
        $$ = new NUnaryOperator(
            std::shared_ptr<NExpression>($2),
            $1
        );
    } */
    /*
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')' */
	;

/* unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	; */

cast_expression
	: unary_expression
	/* | '(' type_name ')' cast_expression */
	;

multiplicative_expression
	: multiplicative_expression MUL_OP cast_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| multiplicative_expression DIV_OP cast_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| multiplicative_expression MOD_OP cast_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression ADD_OP multiplicative_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| additive_expression NEG_OP multiplicative_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| shift_expression RIGHT_OP additive_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	;

relational_expression
	: shift_expression
	| relational_expression LT_OP shift_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| relational_expression GT_OP shift_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| relational_expression LE_OP shift_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| relational_expression GE_OP shift_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
			$2,
			std::shared_ptr<NExpression>($3));
	}
	| equality_expression NE_OP relational_expression {
		$$ = new NBinaryOperator(std::shared_ptr<NExpression>($1),
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
		$$ = new NAssignment(std::shared_ptr<NExpression>($1), std::shared_ptr<NExpression>($3));
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
	: type_specifier init_declarator_list {
		$2->set_type($1);
		$$ = $2;
	}
	;

init_declarator_list
	: init_declarator {
		$$ = new NVariableDeclarationList(std::make_shared<VariableList>());
		$$->push_back(std::shared_ptr<NVariableDeclaration>($1));
	}
	| init_declarator_list ',' init_declarator {
		$1->push_back(std::shared_ptr<NVariableDeclaration>($3));
	};
	;

init_declarator
	: declarator '=' conditional_expression {
		$$ = new NVariableDeclaration(
			0,
			std::shared_ptr<NIdentifier>($1),
			std::shared_ptr<NExpression>($3)
		); 
	}
	| declarator {
		$$ = new NVariableDeclaration(
			0,
			std::shared_ptr<NIdentifier>($1)
		); 
	}
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

declarator
	: pointer direct_declarator { $2->pointer_level = 1; $$ = $2; }
	| direct_declarator
	;

direct_declarator
	: IDENTIFIER { $$ = new NIdentifier(std::shared_ptr<std::string>($1)); }
	/* | '(' declarator ')' */
	| direct_declarator '[' CONSTANT ']' {
        $1->array_size = std::stoi(*$3);
        delete $3;
	}
	/* | direct_declarator '[' ']' */
	;

pointer
	: MUL_OP { $$ = 0; }
	| MUL_OP pointer { $$ = $2 + 1; }
	;

func_declarator
	: IDENTIFIER '(' parameter_list ')' {
		$$ = new NFunctionDeclaration(
			std::make_shared<NIdentifier>(
				std::shared_ptr<std::string>($1)
			),
			std::shared_ptr<NVariableDeclarationList>($3)
		);
	}
	// | IDENTIFIER '(' identifier_list ')'
	// | IDENTIFIER '(' ')'
	;

parameter_list
	: parameter_declaration {
		$$ = new NVariableDeclarationList(std::make_shared<VariableList>());
		$$->push_back(std::shared_ptr<NVariableDeclaration>($1));
	}
	| parameter_list ',' parameter_declaration {
		$1->push_back(std::shared_ptr<NVariableDeclaration>($3));
	};

parameter_declaration
	: type_specifier {
        $$ = new NVariableDeclaration($1, std::shared_ptr<NIdentifier>(nullptr));
    }
	| type_specifier declarator {
        $$ = new NVariableDeclaration($1, std::shared_ptr<NIdentifier>($2));
    }
	;


identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER

declaration_statement
    : declaration ';' {
			$$ = $<stmt>1;
		}
    ;

expression_statement
	: expression ';' { $$ = new NExpressionStatement(std::shared_ptr<NExpression>($1)); }
	/* | ';' { $$ = new NExpressionStatement(std::shared_ptr<NExpression>(nullptr)); } */
	;

selection_statement
	: IF '(' expression ')' compound_statement {
        $$ = new NIfStatement(
            std::shared_ptr<NExpression>($3),
            std::shared_ptr<NBlock>($5)
        );
    }
	/* | IF '(' expression ')' compound_statement ELSE compound_statement {

    } */
	;

iteration_statement
	: FOR '(' expression ';' expression ';' ')' compound_statement {
        $$ = new NForStatement(
            std::shared_ptr<NBlock>($8),
            std::shared_ptr<NExpression>($3),
            std::shared_ptr<NExpression>($5)
        );
    }
	| FOR '(' expression ';' expression ';' expression ')' compound_statement {
        $$ = new NForStatement(
            std::shared_ptr<NBlock>($9),
            std::shared_ptr<NExpression>($3),
            std::shared_ptr<NExpression>($5),
            std::shared_ptr<NExpression>($7)
        );
    }
	/* | WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';' */
	;

jump_statement
	: RETURN expression ';' {
        $$ = new NReturnStatement(
            std::shared_ptr<NExpression>($2)
        );
    }
	/* | RETURN ';' */
	/* | GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';' */
	;

statement
	: declaration_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

statement_list
	: statement {
        $$ = new NBlock();
        $$->push_back(std::shared_ptr<NStatement>($1));
    }
	| statement_list statement {
        $1->push_back(std::shared_ptr<NStatement>($2));
    }
	;

compound_statement
	: '{' '}' { $$ = new NBlock(); }
	| '{' statement_list '}' { $$ = $2; }
	;



translation_unit
	: function_definition { programBlocks.push_back(std::shared_ptr<NFunction>($1)); }
	| translation_unit function_definition { programBlocks.push_back(std::shared_ptr<NFunction>($2)); }
	;

function_definition
	: type_specifier func_declarator compound_statement {
        $$ = new NFunction($1,
        std::shared_ptr<NFunctionDeclaration>($2),
        std::shared_ptr<NBlock>($3));
    }
	;
%%
