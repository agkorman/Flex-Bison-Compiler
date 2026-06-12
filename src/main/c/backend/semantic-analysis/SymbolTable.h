#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"

/** Initialize module's internal state. */
ModuleDestructor initializeSymbolTableModule();

typedef enum SymbolKind SymbolKind;
typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;

enum SymbolKind {
	NETWORK_SYMBOL,
	SERVICE_SYMBOL,
	VOLUME_SYMBOL
};

/**
 * Symbols do not own their strings: they alias memory owned by the AST, so
 * the table must be destroyed before the program (see EntryPoint.c).
 */
struct Symbol {
	SymbolKind kind;
	char * name;
	/** NULL for networks. */
	char * networkName;
	/** NO_ROLE for everything but services. */
	RoleType role;
	Symbol * next;
};

struct SymbolTable {
	Symbol * first;
};

SymbolTable * createSymbolTable();
void destroySymbolTable(SymbolTable * table);

/** Inserts a new symbol; uniqueness must be checked by the caller. */
Symbol * insertSymbol(SymbolTable * table, SymbolKind kind, char * name, char * networkName, RoleType role);

/** Finds a symbol inside the given scope (NULL networkName matches networks). */
Symbol * lookupSymbol(SymbolTable * table, SymbolKind kind, const char * name, const char * networkName);

/** Finds a symbol with the given name in any network. */
Symbol * lookupSymbolAnywhere(SymbolTable * table, SymbolKind kind, const char * name);

#endif
