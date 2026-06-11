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

CompilationStatus AppLexemeAction();
CompilationStatus ArrowLexemeAction();
CompilationStatus AtLexemeAction();
CompilationStatus CacheLexemeAction();
CompilationStatus CloseBraceLexemeAction();
CompilationStatus DatabaseLexemeAction();
CompilationStatus EndLexemeAction();
CompilationStatus ExposeLexemeAction();
CompilationStatus IdentifierLexemeAction();
CompilationStatus IgnoredLexemeAction();
CompilationStatus IntegerLexemeAction();
CompilationStatus MountLexemeAction();
CompilationStatus NetworkLexemeAction();
CompilationStatus OnLexemeAction();
CompilationStatus OpenBraceLexemeAction();
CompilationStatus ProxyLexemeAction();
CompilationStatus SemicolonLexemeAction();
CompilationStatus ServiceLexemeAction();
CompilationStatus StaticLexemeAction();
CompilationStatus StringLexemeAction();
CompilationStatus UnknownLexemeAction();
CompilationStatus UsingLexemeAction();
CompilationStatus VolumeLexemeAction();

#endif
