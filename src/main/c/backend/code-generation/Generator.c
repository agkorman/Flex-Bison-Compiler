#include "Generator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}

/** PUBLIC FUNCTIONS */

void executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	Program * program = compilerState->abstractSyntaxtTree;
	if (program != NULL) {
		logDebugging(_logger, "The program skeleton has no output yet.");
	}
	logDebugging(_logger, "Generation is done.");
}
