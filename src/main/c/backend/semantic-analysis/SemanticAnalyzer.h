#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "SymbolTable.h"

/** Initialize module's internal state. */
ModuleDestructor initializeSemanticAnalyzerModule();

/**
 * Walks the AST in two passes: the first one populates the symbol table with
 * every declared network, service and volume (reporting duplicates); the
 * second one validates every reference and domain rule. On success, the
 * symbol table is left inside the compiler state for later phases.
 */
CompilationStatus executeSemanticAnalysis(CompilerState * compilerState);

#endif
