#include "SymbolTable.h"
#include <stdbool.h>
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownSymbolTableModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: SymbolTable...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeSymbolTableModule() {
	_logger = createLogger("SymbolTable");
	return _shutdownSymbolTableModule;
}

/* PRIVATE FUNCTIONS */

static bool _sameScope(const Symbol * symbol, const char * networkName) {
	if (symbol->networkName == NULL || networkName == NULL) {
		return symbol->networkName == networkName;
	}
	return strcmp(symbol->networkName, networkName) == 0;
}

/* PUBLIC FUNCTIONS */

SymbolTable * createSymbolTable() {
	SymbolTable * table = calloc(1, sizeof(SymbolTable));
	logDebugging(_logger, "Symbol table created.");
	return table;
}

void destroySymbolTable(SymbolTable * table) {
	if (table == NULL) {
		return;
	}
	Symbol * symbol = table->first;
	while (symbol != NULL) {
		Symbol * next = symbol->next;
		free(symbol);
		symbol = next;
	}
	free(table);
	logDebugging(_logger, "Symbol table destroyed.");
}

Symbol * insertSymbol(SymbolTable * table, SymbolKind kind, char * name, char * networkName, RoleType role) {
	Symbol * symbol = calloc(1, sizeof(Symbol));
	symbol->kind = kind;
	symbol->name = name;
	symbol->networkName = networkName;
	symbol->role = role;
	symbol->next = table->first;
	table->first = symbol;
	logDebugging(_logger, "Symbol inserted: \"%s\" (kind %d, network \"%s\").",
		name, kind, networkName == NULL ? "<global>" : networkName);
	return symbol;
}

Symbol * lookupSymbol(SymbolTable * table, SymbolKind kind, const char * name, const char * networkName) {
	for (Symbol * symbol = table->first; symbol != NULL; symbol = symbol->next) {
		if (symbol->kind == kind && strcmp(symbol->name, name) == 0 && _sameScope(symbol, networkName)) {
			return symbol;
		}
	}
	return NULL;
}

Symbol * lookupSymbolAnywhere(SymbolTable * table, SymbolKind kind, const char * name) {
	for (Symbol * symbol = table->first; symbol != NULL; symbol = symbol->next) {
		if (symbol->kind == kind && strcmp(symbol->name, name) == 0) {
			return symbol;
		}
	}
	return NULL;
}
