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
 * Symbols do not own their strings: "name" and "networkName" alias memory
 * owned by the AST, so the symbol table must be destroyed before the program
 * (see the teardown order in EntryPoint.c).
 */
struct Symbol {
	SymbolKind kind;
	char * name;

	/** The owning network for services and volumes; NULL for networks. */
	char * networkName;

	/** Only meaningful for services. */
	RoleType role;

	Symbol * next;
};

struct SymbolTable {
	Symbol * first;
};

SymbolTable * createSymbolTable();
void destroySymbolTable(SymbolTable * table);

/**
 * Inserts a new symbol. Uniqueness is NOT enforced here; callers must check
 * with a lookup before inserting (the semantic analyzer reports duplicates).
 */
Symbol * insertSymbol(SymbolTable * table, SymbolKind kind, char * name, char * networkName, RoleType role);

/**
 * Finds a symbol of the given kind and name inside the given scope. A NULL
 * networkName matches symbols without an owning network (i.e., networks).
 */
Symbol * lookupSymbol(SymbolTable * table, SymbolKind kind, const char * name, const char * networkName);

/**
 * Finds the first symbol of the given kind with the given name in any
 * network (used to resolve cross-network references after the local scope
 * misses, and to enforce global uniqueness of Compose keys).
 */
Symbol * lookupSymbolAnywhere(SymbolTable * table, SymbolKind kind, const char * name);

#endif
