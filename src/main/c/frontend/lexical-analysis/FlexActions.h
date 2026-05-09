#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../support/configuration/Environment.h"
#include "../../support/language/String.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/LexicalAnalyzer.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/Token.h"
#include "../../support/type/TokenLabel.h"
#include "../Frontend.h"

/** Initialize module's internal state. */
ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer);

CompilationStatus ApiLexemeAction();
CompilationStatus AppLexemeAction();
CompilationStatus CacheLexemeAction();
CompilationStatus CloseBraceLexemeAction();
CompilationStatus ConnectsToLexemeAction();
CompilationStatus DatabaseLexemeAction();
CompilationStatus EndLexemeAction();
CompilationStatus ExposeLexemeAction();
CompilationStatus FrontendLexemeAction();
CompilationStatus IdentifierLexemeAction();
CompilationStatus IgnoredLexemeAction();
CompilationStatus IntegerLexemeAction();
CompilationStatus OnLexemeAction();
CompilationStatus OpenBraceLexemeAction();
CompilationStatus SemicolonLexemeAction();
CompilationStatus StringLexemeAction();
CompilationStatus UnknownLexemeAction();
CompilationStatus UsingLexemeAction();

#endif
