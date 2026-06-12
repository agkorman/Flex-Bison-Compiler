#include "SemanticAnalyzer.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownSemanticAnalyzerModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: SemanticAnalyzer...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeSemanticAnalyzerModule() {
	_logger = createLogger("SemanticAnalyzer");
	return _shutdownSemanticAnalyzerModule;
}

/* PRIVATE FUNCTIONS */

static unsigned int _errorCount = 0;

static void _reportError(const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * effectiveFormat = concatenate(2, "Semantic error: ", format);
	// Reuse the logger machinery through a single formatted message.
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), effectiveFormat, arguments);
	logError(_logger, "%s", buffer);
	free(effectiveFormat);
	va_end(arguments);
	++_errorCount;
}

/**
 * First pass: declare every network, service and volume in the symbol table,
 * reporting duplicate declarations within the same scope.
 */
static void _declareSymbols(SymbolTable * table, App * app) {
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type != NETWORK_ITEM) {
			continue;
		}
		Network * network = item->network;
		if (lookupSymbol(table, NETWORK_SYMBOL, network->name, NULL) != NULL) {
			_reportError("duplicate network \"%s\" in app \"%s\".", network->name, app->name);
		}
		else {
			insertSymbol(table, NETWORK_SYMBOL, network->name, NULL, SERVICE_ROLE);
		}
		for (Declaration * declaration = network->declarations; declaration != NULL; declaration = declaration->next) {
			switch (declaration->type) {
				case SERVICE_DECLARATION: {
					ServiceDeclaration * service = declaration->service;
					if (lookupSymbol(table, SERVICE_SYMBOL, service->name, network->name) != NULL) {
						_reportError("duplicate service \"%s\" in network \"%s\".", service->name, network->name);
					}
					else {
						Symbol * homonym = lookupServiceAnywhere(table, service->name);
						if (homonym != NULL) {
							// A repeated name across networks would collide as a
							// Compose service key, so it is rejected outright.
							_reportError("duplicate service \"%s\": already declared in network \"%s\".", service->name, homonym->networkName);
						}
						else {
							insertSymbol(table, SERVICE_SYMBOL, service->name, network->name, service->role);
						}
					}
					break;
				}
				case VOLUME_DECLARATION: {
					VolumeDeclaration * volume = declaration->volume;
					if (lookupSymbol(table, VOLUME_SYMBOL, volume->name, network->name) != NULL) {
						_reportError("duplicate volume \"%s\" in network \"%s\".", volume->name, network->name);
					}
					else {
						insertSymbol(table, VOLUME_SYMBOL, volume->name, network->name, SERVICE_ROLE);
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

/** True if an app-level link "from -> to" exists between both networks. */
static int _networksAreLinked(App * app, const char * fromNetwork, const char * toNetwork) {
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type != NETWORK_CONNECT_ITEM) {
			continue;
		}
		if (strcmp(item->connect->from, fromNetwork) == 0 && strcmp(item->connect->to, toNetwork) == 0) {
			return 1;
		}
	}
	return 0;
}

/**
 * Resolves a service reference from within a network: the local scope wins,
 * and only then the rest of the networks are searched (cross-network use).
 */
static Symbol * _resolveService(SymbolTable * table, const char * name, const char * networkName) {
	Symbol * local = lookupSymbol(table, SERVICE_SYMBOL, name, networkName);
	if (local != NULL) {
		return local;
	}
	return lookupServiceAnywhere(table, name);
}

static void _validateExpose(SymbolTable * table, Network * network, ExposeDeclaration * expose) {
	if (expose->port < 1 || 65535 < expose->port) {
		_reportError("invalid port %d for service \"%s\" (must be between 1 and 65535).", expose->port, expose->serviceName);
	}
	Symbol * service = lookupSymbol(table, SERVICE_SYMBOL, expose->serviceName, network->name);
	if (service == NULL) {
		_reportError("cannot expose undeclared service \"%s\" in network \"%s\".", expose->serviceName, network->name);
		return;
	}
	if (service->role == DATABASE_ROLE || service->role == CACHE_ROLE) {
		_reportError("service \"%s\" cannot be exposed: databases and caches are internal by design.", expose->serviceName);
	}
}

static void _validateConnect(SymbolTable * table, App * app, Network * network, ConnectDeclaration * connect) {
	Symbol * from = _resolveService(table, connect->from, network->name);
	Symbol * to = _resolveService(table, connect->to, network->name);
	if (from == NULL) {
		_reportError("connection \"%s -> %s\" references undeclared service \"%s\".", connect->from, connect->to, connect->from);
	}
	if (to == NULL) {
		_reportError("connection \"%s -> %s\" references undeclared service \"%s\".", connect->from, connect->to, connect->to);
	}
	if (from == NULL || to == NULL) {
		return;
	}
	if (strcmp(from->networkName, to->networkName) != 0
		&& !_networksAreLinked(app, from->networkName, to->networkName)) {
		_reportError("service \"%s\" (network \"%s\") cannot reach \"%s\" (network \"%s\") without an app-level link \"%s -> %s;\".",
			from->name, from->networkName, to->name, to->networkName, from->networkName, to->networkName);
	}
}

static void _validateMount(SymbolTable * table, Network * network, MountDeclaration * mount) {
	if (lookupSymbol(table, SERVICE_SYMBOL, mount->serviceName, network->name) == NULL) {
		_reportError("cannot mount \"%s\" on undeclared service \"%s\" in network \"%s\".", mount->source, mount->serviceName, network->name);
	}
	if (mount->sourceType == VOLUME_SOURCE
		&& lookupSymbol(table, VOLUME_SYMBOL, mount->source, network->name) == NULL) {
		_reportError("mount references undeclared volume \"%s\" in network \"%s\".", mount->source, network->name);
	}
}

static void _validateNetworkLink(SymbolTable * table, ConnectDeclaration * link) {
	if (lookupSymbol(table, NETWORK_SYMBOL, link->from, NULL) == NULL) {
		_reportError("network link \"%s -> %s\" references undeclared network \"%s\".", link->from, link->to, link->from);
	}
	if (lookupSymbol(table, NETWORK_SYMBOL, link->to, NULL) == NULL) {
		_reportError("network link \"%s -> %s\" references undeclared network \"%s\".", link->from, link->to, link->to);
	}
}

/** Second pass: validate every reference and domain rule. */
static void _validateReferences(SymbolTable * table, App * app) {
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type == NETWORK_CONNECT_ITEM) {
			_validateNetworkLink(table, item->connect);
			continue;
		}
		Network * network = item->network;
		for (Declaration * declaration = network->declarations; declaration != NULL; declaration = declaration->next) {
			switch (declaration->type) {
				case EXPOSE_DECLARATION:
					_validateExpose(table, network, declaration->expose);
					break;
				case CONNECT_DECLARATION:
					_validateConnect(table, app, network, declaration->connect);
					break;
				case MOUNT_DECLARATION:
					_validateMount(table, network, declaration->mount);
					break;
				default:
					break;
			}
		}
	}
}

/* PUBLIC FUNCTIONS */

CompilationStatus executeSemanticAnalysis(CompilerState * compilerState) {
	logDebugging(_logger, "Executing semantic analysis...");
	Program * program = compilerState->abstractSyntaxtTree;
	if (program == NULL || program->app == NULL) {
		logError(_logger, "There is no abstract syntax tree to analyze.");
		return FAILED;
	}
	_errorCount = 0;
	SymbolTable * table = createSymbolTable();
	compilerState->symbolTable = table;
	_declareSymbols(table, program->app);
	_validateReferences(table, program->app);
	if (0 < _errorCount) {
		logError(_logger, "The semantic-analysis phase found %d error(s).", _errorCount);
		return FAILED;
	}
	logDebugging(_logger, "Semantic analysis is done.");
	return SUCCEEDED;
}
