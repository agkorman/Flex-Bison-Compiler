#include "Generator.h"
#include "../semantic-analysis/SymbolTable.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* MODULE INTERNAL STATE */

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

/* PRIVATE FUNCTIONS */

static void _output(FILE * file, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	vfprintf(file, format, arguments);
	va_end(arguments);
}

static int _makeDirectory(const char * path) {
	if (mkdir(path, 0755) != 0 && errno != EEXIST) {
		logError(_logger, "Cannot create directory \"%s\".", path);
		return 0;
	}
	return 1;
}

static const char * _roleName(RoleType role) {
	switch (role) {
		case PROXY_ROLE: return "proxy";
		case SERVICE_ROLE: return "service";
		case STATIC_ROLE: return "static";
		case DATABASE_ROLE: return "database";
		case CACHE_ROLE: return "cache";
		default: return "unknown";
	}
}

/** Appends a network name to the list if it is not already there. */
static void _addNetwork(const char ** networks, unsigned int * count, const char * name) {
	for (unsigned int k = 0; k < *count; ++k) {
		if (strcmp(networks[k], name) == 0) {
			return;
		}
	}
	networks[*count] = name;
	++(*count);
}

static unsigned int _countNetworks(App * app) {
	unsigned int count = 0;
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type == NETWORK_ITEM) {
			++count;
		}
	}
	return count;
}

/**
 * Networks a service belongs to: its own, the ones of its targets, and for
 * proxies, the targets of the app-level links starting at its network.
 */
static unsigned int _serviceNetworks(App * app, SymbolTable * table, Network * network,
	ServiceDeclaration * service, const char ** networks) {
	unsigned int count = 0;
	_addNetwork(networks, &count, network->name);
	for (Declaration * declaration = network->declarations; declaration != NULL; declaration = declaration->next) {
		if (declaration->type != CONNECT_DECLARATION
			|| strcmp(declaration->connect->from, service->name) != 0) {
			continue;
		}
		Symbol * target = lookupSymbol(table, SERVICE_SYMBOL, declaration->connect->to, network->name);
		if (target == NULL) {
			target = lookupSymbolAnywhere(table, SERVICE_SYMBOL, declaration->connect->to);
		}
		if (target != NULL) {
			_addNetwork(networks, &count, target->networkName);
		}
	}
	if (service->role == PROXY_ROLE) {
		for (AppItem * item = app->items; item != NULL; item = item->next) {
			if (item->type == NETWORK_CONNECT_ITEM
				&& strcmp(item->connect->from, network->name) == 0) {
				_addNetwork(networks, &count, item->connect->to);
			}
		}
	}
	return count;
}

static void _generateService(FILE * file, App * app, SymbolTable * table,
	Network * network, ServiceDeclaration * service) {
	_output(file, "    %s:\n", service->name);
	_output(file, "        image: \"%s\"\n", service->image);
	const char ** networks = calloc(_countNetworks(app), sizeof(char *));
	unsigned int networkCount = _serviceNetworks(app, table, network, service, networks);
	_output(file, "        networks:\n");
	for (unsigned int k = 0; k < networkCount; ++k) {
		_output(file, "            - %s\n", networks[k]);
	}
	free(networks);
	int header = 0;
	for (Declaration * declaration = network->declarations; declaration != NULL; declaration = declaration->next) {
		if (declaration->type == EXPOSE_DECLARATION
			&& strcmp(declaration->expose->serviceName, service->name) == 0) {
			if (!header) {
				_output(file, "        ports:\n");
				header = 1;
			}
			_output(file, "            - \"%d:%d\"\n", declaration->expose->port, declaration->expose->port);
		}
	}
	header = 0;
	for (Declaration * declaration = network->declarations; declaration != NULL; declaration = declaration->next) {
		if (declaration->type == CONNECT_DECLARATION
			&& strcmp(declaration->connect->from, service->name) == 0) {
			if (!header) {
				_output(file, "        depends_on:\n");
				header = 1;
			}
			_output(file, "            - %s\n", declaration->connect->to);
		}
	}
	header = 0;
	for (Declaration * declaration = network->declarations; declaration != NULL; declaration = declaration->next) {
		if (declaration->type == MOUNT_DECLARATION
			&& strcmp(declaration->mount->serviceName, service->name) == 0) {
			if (!header) {
				_output(file, "        volumes:\n");
				header = 1;
			}
			// Paths are quoted so YAML special characters cannot break the file.
			if (declaration->mount->sourceType == HOST_PATH_SOURCE) {
				_output(file, "            - type: bind\n");
				_output(file, "              source: \"%s\"\n", declaration->mount->source);
				_output(file, "              target: \"%s\"\n", declaration->mount->containerPath);
			}
			else {
				_output(file, "            - \"%s:%s\"\n", declaration->mount->source, declaration->mount->containerPath);
			}
		}
	}
}

static void _generateComposeFile(FILE * file, App * app, SymbolTable * table) {
	_output(file, "# Generated by StackForge for app \"%s\". Do not edit by hand.\n", app->name);
	_output(file, "services:\n");
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type != NETWORK_ITEM) {
			continue;
		}
		for (Declaration * declaration = item->network->declarations; declaration != NULL; declaration = declaration->next) {
			if (declaration->type == SERVICE_DECLARATION) {
				_generateService(file, app, table, item->network, declaration->service);
			}
		}
	}
	_output(file, "networks:\n");
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type == NETWORK_ITEM) {
			_output(file, "    %s: {}\n", item->network->name);
		}
	}
	int header = 0;
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type != NETWORK_ITEM) {
			continue;
		}
		for (Declaration * declaration = item->network->declarations; declaration != NULL; declaration = declaration->next) {
			if (declaration->type == VOLUME_DECLARATION) {
				if (!header) {
					_output(file, "volumes:\n");
					header = 1;
				}
				_output(file, "    %s: {}\n", declaration->volume->name);
			}
		}
	}
}

/** Emits a per-service folder with a minimal Dockerfile stub. */
static CompilationStatus _generateScaffolding(const char * appPath, App * app) {
	CompilationStatus status = SUCCEEDED;
	char * servicesPath = concatenate(2, appPath, "/services");
	if (!_makeDirectory(servicesPath)) {
		free(servicesPath);
		return FAILED;
	}
	for (AppItem * item = app->items; item != NULL; item = item->next) {
		if (item->type != NETWORK_ITEM) {
			continue;
		}
		for (Declaration * declaration = item->network->declarations; declaration != NULL; declaration = declaration->next) {
			if (declaration->type != SERVICE_DECLARATION) {
				continue;
			}
			ServiceDeclaration * service = declaration->service;
			char * servicePath = concatenate(3, servicesPath, "/", service->name);
			if (_makeDirectory(servicePath)) {
				char * dockerfilePath = concatenate(2, servicePath, "/Dockerfile");
				FILE * dockerfile = fopen(dockerfilePath, "w");
				if (dockerfile != NULL) {
					_output(dockerfile, "# Scaffolding for %s \"%s\", generated by StackForge.\n",
						_roleName(service->role), service->name);
					_output(dockerfile, "FROM %s\n", service->image);
					switch (service->role) {
						case PROXY_ROLE:
							_output(dockerfile, "\n# Drop your reverse-proxy rules here:\n");
							_output(dockerfile, "# COPY nginx.conf /etc/nginx/nginx.conf\n");
							break;
						case STATIC_ROLE:
							_output(dockerfile, "\n# Drop your static assets here:\n");
							_output(dockerfile, "# COPY ./public /usr/share/nginx/html\n");
							break;
						case SERVICE_ROLE:
							_output(dockerfile, "\nWORKDIR /app\n");
							_output(dockerfile, "\n# Drop your application here:\n");
							_output(dockerfile, "# COPY . .\n");
							_output(dockerfile, "# CMD [\"...\"]\n");
							break;
						default:
							_output(dockerfile, "\n# This %s is internal by design; configure it via environment variables.\n",
								_roleName(service->role));
							break;
					}
					fclose(dockerfile);
				}
				else {
					logError(_logger, "Cannot create \"%s\".", dockerfilePath);
					status = FAILED;
				}
				free(dockerfilePath);
			}
			else {
				status = FAILED;
			}
			free(servicePath);
		}
	}
	free(servicesPath);
	return status;
}

/* PUBLIC FUNCTIONS */

CompilationStatus executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	Program * program = compilerState->abstractSyntaxtTree;
	SymbolTable * table = compilerState->symbolTable;
	if (program == NULL || program->app == NULL || table == NULL) {
		logError(_logger, "There is no validated program to generate.");
		return FAILED;
	}
	App * app = program->app;
	if (!_makeDirectory("output")) {
		return FAILED;
	}
	char * appPath = concatenate(2, "output/", app->name);
	if (!_makeDirectory(appPath)) {
		free(appPath);
		return FAILED;
	}
	char * composePath = concatenate(2, appPath, "/docker-compose.yml");
	FILE * composeFile = fopen(composePath, "w");
	if (composeFile == NULL) {
		logError(_logger, "Cannot create \"%s\".", composePath);
		free(composePath);
		free(appPath);
		return FAILED;
	}
	_generateComposeFile(composeFile, app, table);
	fclose(composeFile);
	CompilationStatus status = _generateScaffolding(appPath, app);
	if (status == SUCCEEDED) {
		logInformation(_logger, "Artifacts generated under \"%s\".", appPath);
	}
	free(composePath);
	free(appPath);
	logDebugging(_logger, "Generation is done.");
	return status;
}
