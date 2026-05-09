%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
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

	RoleType role;
	App * app;
	Declaration * declaration;
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
%destructor { destroyDeclaration($$); } <declaration>
%destructor { destroyApp($$); } <app>

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
%type <role> role
%type <app> app
%type <declaration> declaration
%type <declaration> declarations
%type <program> program

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: app												{ $$ = AppProgramSemanticAction($1); }
	;

app: APP ID OPEN_BRACE declarations CLOSE_BRACE			{ $$ = AppSemanticAction($2, $4); }
	;

declarations: declarations declaration					{ $$ = AppendDeclarationSemanticAction($1, $2); }
	| declaration										{ $$ = SingleDeclarationSemanticAction($1); }
	;

declaration: role ID USING STRING SEMICOLON				{ $$ = ServiceDeclarationSemanticAction($1, $2, $4); }
	| EXPOSE ID ON INTEGER SEMICOLON					{ $$ = ExposeDeclarationSemanticAction($2, $4); }
	| ID CONNECTS_TO ID SEMICOLON						{ $$ = ConnectDeclarationSemanticAction($1, $3); }
	;

role: FRONTEND											{ $$ = FRONTEND_ROLE; }
	| API												{ $$ = API_ROLE; }
	| DATABASE											{ $$ = DATABASE_ROLE; }
	| CACHE												{ $$ = CACHE_ROLE; }
	;

%%
