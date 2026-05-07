#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdbool.h>
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

typedef struct Program Program;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

struct Program {
	bool empty;
};

/**
 * Node destructors.
 */

void destroyProgram(Program * program);

#endif
