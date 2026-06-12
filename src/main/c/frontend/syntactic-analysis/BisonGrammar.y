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
	AppItem * appItem;
	Network * network;
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
%destructor { destroyNetwork($$); } <network>
%destructor { destroyAppItem($$); } <appItem>
%destructor { destroyApp($$); } <app>

/** Terminals. */
%token <token> APP
%token <token> NETWORK
%token <token> PROXY
%token <token> SERVICE
%token <token> STATIC
%token <token> DATABASE
%token <token> CACHE
%token <token> USING
%token <token> EXPOSE
%token <token> ON
%token <token> VOLUME
%token <token> MOUNT
%token <token> AT
%token <token> ARROW

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
%type <appItem> appItem
%type <appItem> appItems
%type <network> network
%type <declaration> declaration
%type <declaration> declarations
%type <program> program

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: app												{ $$ = AppProgramSemanticAction($1); }
	;

app: APP ID OPEN_BRACE appItems CLOSE_BRACE				{ $$ = AppSemanticAction($2, $4); }
	;

appItems: appItems appItem								{ $$ = AppendAppItemSemanticAction($1, $2); }
	| appItem											{ $$ = SingleAppItemSemanticAction($1); }
	;

appItem: network										{ $$ = NetworkAppItemSemanticAction($1); }
	| ID ARROW ID SEMICOLON								{ $$ = NetworkConnectAppItemSemanticAction($1, $3); }
	;

network: NETWORK ID OPEN_BRACE declarations CLOSE_BRACE	{ $$ = NetworkSemanticAction($2, $4); }
	;

declarations: declarations declaration					{ $$ = AppendDeclarationSemanticAction($1, $2); }
	| declaration										{ $$ = SingleDeclarationSemanticAction($1); }
	;

declaration: role ID USING STRING SEMICOLON				{ $$ = ServiceDeclarationSemanticAction($1, $2, $4); }
	| EXPOSE ID ON INTEGER SEMICOLON					{ $$ = ExposeDeclarationSemanticAction($2, $4); }
	| ID ARROW ID SEMICOLON								{ $$ = ConnectDeclarationSemanticAction($1, $3); }
	| VOLUME ID SEMICOLON								{ $$ = VolumeDeclarationSemanticAction($2); }
	| MOUNT ID ON ID AT STRING SEMICOLON				{ $$ = MountDeclarationSemanticAction(VOLUME_SOURCE, $2, $4, $6); }
	| MOUNT STRING ON ID AT STRING SEMICOLON			{ $$ = MountDeclarationSemanticAction(HOST_PATH_SOURCE, $2, $4, $6); }
	;

role: PROXY												{ $$ = PROXY_ROLE; }
	| SERVICE											{ $$ = SERVICE_ROLE; }
	| STATIC											{ $$ = STATIC_ROLE; }
	| DATABASE											{ $$ = DATABASE_ROLE; }
	| CACHE												{ $$ = CACHE_ROLE; }
	;

%%
