#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"

/** Initialize module's internal state. */
ModuleDestructor initializeGeneratorModule();

/**
 * Generates the final output using the current compiler state. Fails when
 * any artifact cannot be written, so a partial output tree is never reported
 * as a successful compilation.
 */
CompilationStatus executeGenerator(CompilerState * compilerState);

#endif
