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
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), effectiveFormat, arguments);
	logError(_logger, "%s", buffer);
	free(effectiveFormat);
	va_end(arguments);
	++_errorCount;
}

static void _validateServiceDeclarations(App * app) {
	unsigned int serviceCount = 0;
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type != NETWORK_ITEM) {
			continue;
		}
		for (Declaration * declaration = item->network->declarations; declaration != NULL; declaration = declaration->next) {
			if (declaration->type != SERVICE_DECLARATION) {
				continue;
			}
			++serviceCount;
			if (declaration->service->image == NULL || declaration->service->image[0] == '\0') {
				_reportError("service \"%s\" in network \"%s\" must use a non-empty image.",
					declaration->service->name, item->network->name);
			}
		}
	}
	if (serviceCount == 0) {
		_reportError("app \"%s\" must declare at least one service.", app->name);
	}
}

/** First pass: declare every network, service and volume, rejecting duplicates. */
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
			insertSymbol(table, NETWORK_SYMBOL, network->name, NULL, NO_ROLE);
		}
		for (Declaration * declaration = network->declarations; declaration != NULL; declaration = declaration->next) {
			switch (declaration->type) {
				case SERVICE_DECLARATION: {
					ServiceDeclaration * service = declaration->service;
					if (lookupSymbol(table, SERVICE_SYMBOL, service->name, network->name) != NULL) {
						_reportError("duplicate service \"%s\" in network \"%s\".", service->name, network->name);
					}
					else {
						// Service names are global: they become Compose service keys.
						Symbol * homonym = lookupSymbolAnywhere(table, SERVICE_SYMBOL, service->name);
						if (homonym != NULL) {
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
						// Volume names are global too: they become Compose volume keys.
						Symbol * homonym = lookupSymbolAnywhere(table, VOLUME_SYMBOL, volume->name);
						if (homonym != NULL) {
							_reportError("duplicate volume \"%s\": already declared in network \"%s\".", volume->name, homonym->networkName);
						}
						else {
							insertSymbol(table, VOLUME_SYMBOL, volume->name, network->name, NO_ROLE);
						}
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

/** Resolves a service reference: local scope first, then any other network. */
static Symbol * _resolveService(SymbolTable * table, const char * name, const char * networkName) {
	Symbol * local = lookupSymbol(table, SERVICE_SYMBOL, name, networkName);
	if (local != NULL) {
		return local;
	}
	return lookupSymbolAnywhere(table, SERVICE_SYMBOL, name);
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

/** Same service on the same port twice would duplicate "ports:" entries. */
static void _checkDuplicateExpose(Network * network, Declaration * current) {
	ExposeDeclaration * expose = current->expose;
	for (Declaration * declaration = network->declarations; declaration != current; declaration = declaration->next) {
		if (declaration->type == EXPOSE_DECLARATION
			&& strcmp(declaration->expose->serviceName, expose->serviceName) == 0
			&& declaration->expose->port == expose->port) {
			_reportError("duplicate expose of service \"%s\" on port %d in network \"%s\".",
				expose->serviceName, expose->port, network->name);
			return;
		}
	}
}

/** The same connection twice would duplicate "depends_on" entries. */
static void _checkDuplicateConnect(Network * network, Declaration * current) {
	ConnectDeclaration * connect = current->connect;
	for (Declaration * declaration = network->declarations; declaration != current; declaration = declaration->next) {
		if (declaration->type == CONNECT_DECLARATION
			&& strcmp(declaration->connect->from, connect->from) == 0
			&& strcmp(declaration->connect->to, connect->to) == 0) {
			_reportError("duplicate connection \"%s -> %s\" in network \"%s\".",
				connect->from, connect->to, network->name);
			return;
		}
	}
}

/** Two services publishing the same host port would conflict at "docker compose up". */
static void _checkPortConflict(App * app, Declaration * current) {
	ExposeDeclaration * expose = current->expose;
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type != NETWORK_ITEM) {
			continue;
		}
		for (Declaration * declaration = item->network->declarations; declaration != NULL; declaration = declaration->next) {
			if (declaration == current) {
				return;
			}
			if (declaration->type == EXPOSE_DECLARATION
				&& declaration->expose->port == expose->port
				&& strcmp(declaration->expose->serviceName, expose->serviceName) != 0) {
				_reportError("port %d is already published by service \"%s\": it cannot be shared with \"%s\".",
					expose->port, declaration->expose->serviceName, expose->serviceName);
				return;
			}
		}
	}
}

/** The same mount twice would duplicate "volumes:" entries. */
static void _checkDuplicateMount(Network * network, Declaration * current) {
	MountDeclaration * mount = current->mount;
	for (Declaration * declaration = network->declarations; declaration != current; declaration = declaration->next) {
		if (declaration->type == MOUNT_DECLARATION
			&& declaration->mount->sourceType == mount->sourceType
			&& strcmp(declaration->mount->source, mount->source) == 0
			&& strcmp(declaration->mount->serviceName, mount->serviceName) == 0
			&& strcmp(declaration->mount->containerPath, mount->containerPath) == 0) {
			_reportError("duplicate mount of \"%s\" on service \"%s\" at \"%s\" in network \"%s\".",
				mount->source, mount->serviceName, mount->containerPath, network->name);
			return;
		}
	}
}

static void _validateConnect(SymbolTable * table, App * app, Network * network, ConnectDeclaration * connect) {
	// Compose rejects a service that depends on itself.
	if (strcmp(connect->from, connect->to) == 0) {
		_reportError("service \"%s\" cannot connect to itself.", connect->from);
		return;
	}
	// The source must live in this network, or the generator would drop the connection.
	Symbol * from = lookupSymbol(table, SERVICE_SYMBOL, connect->from, network->name);
	Symbol * to = _resolveService(table, connect->to, network->name);
	if (from == NULL) {
		Symbol * foreign = lookupSymbolAnywhere(table, SERVICE_SYMBOL, connect->from);
		if (foreign != NULL) {
			_reportError("connection \"%s -> %s\" must be declared in network \"%s\", where its source \"%s\" lives (not in \"%s\").",
				connect->from, connect->to, foreign->networkName, connect->from, network->name);
		}
		else {
			_reportError("connection \"%s -> %s\" references undeclared service \"%s\".", connect->from, connect->to, connect->from);
		}
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
					_checkDuplicateExpose(network, declaration);
					_checkPortConflict(app, declaration);
					break;
				case CONNECT_DECLARATION:
					_validateConnect(table, app, network, declaration->connect);
					_checkDuplicateConnect(network, declaration);
					break;
				case MOUNT_DECLARATION:
					_validateMount(table, network, declaration->mount);
					_checkDuplicateMount(network, declaration);
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
	_validateServiceDeclarations(program->app);
	_declareSymbols(table, program->app);
	_validateReferences(table, program->app);
	if (0 < _errorCount) {
		logError(_logger, "The semantic-analysis phase found %u error(s).", _errorCount);
		return FAILED;
	}
	logDebugging(_logger, "Semantic analysis is done.");
	return SUCCEEDED;
}
