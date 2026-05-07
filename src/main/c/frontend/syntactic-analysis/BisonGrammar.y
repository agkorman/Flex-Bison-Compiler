%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"
#include <stdlib.h>

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	char * string;
	TokenLabel token;

	/** Non-terminals. */

	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { free($$); } <string>

/** Terminals. */
%token <token> APP
%token <token> FRONTEND
%token <token> API
%token <token> DATABASE
%token <token> CACHE
%token <token> USING
%token <token> EXPOSE
%token <token> ON
%token <token> CONNECTS_TO

%token <string> ID
%token <string> STRING
%token <integer> INTEGER

%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> SEMICOLON

%token END 0 "end of file"

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: %empty												{ $$ = EmptyProgramSemanticAction(); }
	;

%%
