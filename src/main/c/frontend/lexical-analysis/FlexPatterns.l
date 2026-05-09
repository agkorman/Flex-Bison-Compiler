%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus ApiLexemeAction();
extern CompilationStatus AppLexemeAction();
extern CompilationStatus CacheLexemeAction();
extern CompilationStatus CloseBraceLexemeAction();
extern CompilationStatus ConnectsToLexemeAction();
extern CompilationStatus DatabaseLexemeAction();
extern CompilationStatus EndLexemeAction();
extern CompilationStatus ExposeLexemeAction();
extern CompilationStatus FrontendLexemeAction();
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus OnLexemeAction();
extern CompilationStatus OpenBraceLexemeAction();
extern CompilationStatus SemicolonLexemeAction();
extern CompilationStatus StringLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus UsingLexemeAction();

%}

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+
identifier			[A-Za-z_][A-Za-z0-9_]*
integer				[0-9]+
string				\"[^\"\n]*\"
lineComment			"//"[^\n]*

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"app"								{ return AppLexemeAction(); }
"frontend"							{ return FrontendLexemeAction(); }
"api"								{ return ApiLexemeAction(); }
"database"							{ return DatabaseLexemeAction(); }
"cache"								{ return CacheLexemeAction(); }
"using"								{ return UsingLexemeAction(); }
"expose"							{ return ExposeLexemeAction(); }
"on"								{ return OnLexemeAction(); }
"connects_to"						{ return ConnectsToLexemeAction(); }

"{"									{ return OpenBraceLexemeAction(); }
"}"									{ return CloseBraceLexemeAction(); }
";"									{ return SemicolonLexemeAction(); }

{string}							{ return StringLexemeAction(); }
{integer}							{ return IntegerLexemeAction(); }
{identifier}						{ return IdentifierLexemeAction(); }

{lineComment}						{ return IgnoredLexemeAction(); }
{whitespace}						{ return IgnoredLexemeAction(); }

.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EndLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
