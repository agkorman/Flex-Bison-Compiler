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

extern CompilationStatus AppLexemeAction();
extern CompilationStatus ArrowLexemeAction();
extern CompilationStatus AtLexemeAction();
extern CompilationStatus CacheLexemeAction();
extern CompilationStatus CloseBraceLexemeAction();
extern CompilationStatus DatabaseLexemeAction();
extern CompilationStatus EndLexemeAction();
extern CompilationStatus ExposeLexemeAction();
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus MountLexemeAction();
extern CompilationStatus NetworkLexemeAction();
extern CompilationStatus OnLexemeAction();
extern CompilationStatus OpenBraceLexemeAction();
extern CompilationStatus ProxyLexemeAction();
extern CompilationStatus SemicolonLexemeAction();
extern CompilationStatus ServiceLexemeAction();
extern CompilationStatus StaticLexemeAction();
extern CompilationStatus StringLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus UsingLexemeAction();
extern CompilationStatus VolumeLexemeAction();

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
"network"							{ return NetworkLexemeAction(); }
"proxy"								{ return ProxyLexemeAction(); }
"service"							{ return ServiceLexemeAction(); }
"static"							{ return StaticLexemeAction(); }
"database"							{ return DatabaseLexemeAction(); }
"cache"								{ return CacheLexemeAction(); }
"using"								{ return UsingLexemeAction(); }
"expose"							{ return ExposeLexemeAction(); }
"on"								{ return OnLexemeAction(); }
"volume"							{ return VolumeLexemeAction(); }
"mount"								{ return MountLexemeAction(); }
"at"								{ return AtLexemeAction(); }

"->"								{ return ArrowLexemeAction(); }
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
