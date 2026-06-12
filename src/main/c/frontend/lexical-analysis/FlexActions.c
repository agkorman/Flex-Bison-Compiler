#include "FlexActions.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/* MODULE INTERNAL STATE */

static bool _logIgnoredLexemes = true;
static LexicalAnalyzer * _lexicalAnalyzer = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownFlexActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: FlexActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_lexicalAnalyzer = NULL;
}

ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer) {
	_lexicalAnalyzer = lexicalAnalyzer;
	_logger = createLogger("FlexActions");
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	return _shutdownFlexActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logTokenAction(const char * actionName, Token * token);
static char * _copyLexeme(const Token * token);
static char * _copyStringContent(const Token * token);

/**
 * Logs a lexical-analyzer action over a token in DEBUGGING level.
 */
static void _logTokenAction(const char * actionName, Token * token) {
	char * _lexeme = escape(token->lexeme);
	logDebugging(_logger, WARNING_COLOR "%s" DEFAULT_COLOR ": Token(context=%d, label=%d, length=%d, lexeme=%s\"%s\"%s, line=%d, semanticValue=%p)",
		actionName,
		token->context,
		token->label,
		token->length,
		INFORMATION_COLOR, _lexeme, DEFAULT_COLOR,
		token->line,
		token->semanticValue);
	free(_lexeme);
	_lexeme = NULL;
}

/**
 * Creates a heap-owned copy of a token lexeme.
 */
static char * _copyLexeme(const Token * token) {
	char * copy = calloc(1 + token->length, sizeof(char));
	strncpy(copy, token->lexeme, token->length);
	return copy;
}

/**
 * Creates a heap-owned copy of a string token without its quotes.
 */
static char * _copyStringContent(const Token * token) {
	const unsigned int contentLength = token->length - 2;
	char * copy = calloc(1 + contentLength, sizeof(char));
	strncpy(copy, token->lexeme + 1, contentLength);
	return copy;
}

/* PUBLIC FUNCTIONS */

CompilationStatus AppLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, APP);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus ArrowLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, ARROW);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus AtLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, AT);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus CacheLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, CACHE);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus CloseBraceLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, CLOSE_BRACE);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus DatabaseLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, DATABASE);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus EndLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, END);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus ExposeLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, EXPOSE);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus IdentifierLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, ID);
	token->semanticValue->string = _copyLexeme(token);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus IgnoredLexemeAction() {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, IGNORED);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus IntegerLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, INTEGER);
	// An out-of-range literal must not wrap into the valid range (atoi is
	// undefined on overflow), so it is stored as -1 and rejected later by the
	// semantic checks that consume integers.
	errno = 0;
	long value = strtol(token->lexeme, NULL, 10);
	if (errno == ERANGE || INT_MAX < value) {
		value = -1;
	}
	token->semanticValue->integer = (int) value;
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus MountLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, MOUNT);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus NetworkLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, NETWORK);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus OnLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, ON);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus OpenBraceLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, OPEN_BRACE);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus ProxyLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, PROXY);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus SemicolonLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, SEMICOLON);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus ServiceLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, SERVICE);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus StaticLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, STATIC);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus StringLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, STRING);
	token->semanticValue->string = _copyStringContent(token);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus UnknownLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, UNKNOWN);
	_logTokenAction(__FUNCTION__, token);
	destroyToken(token);
	return FAILED;
}

CompilationStatus UsingLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, USING);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus VolumeLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, VOLUME);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}
