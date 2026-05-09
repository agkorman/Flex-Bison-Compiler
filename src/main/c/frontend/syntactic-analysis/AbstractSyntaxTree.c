#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyServiceDeclaration(ServiceDeclaration * service) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (service != NULL) {
		free(service->name);
		free(service->image);
		free(service);
	}
}

void destroyExposeDeclaration(ExposeDeclaration * expose) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expose != NULL) {
		free(expose->serviceName);
		free(expose);
	}
}

void destroyConnectDeclaration(ConnectDeclaration * connect) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (connect != NULL) {
		free(connect->from);
		free(connect->to);
		free(connect);
	}
}

void destroyDeclaration(Declaration * declaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (declaration != NULL) {
		Declaration * next = declaration->next;
		switch (declaration->type) {
			case SERVICE_DECLARATION:
				destroyServiceDeclaration(declaration->service);
				break;
			case EXPOSE_DECLARATION:
				destroyExposeDeclaration(declaration->expose);
				break;
			case CONNECT_DECLARATION:
				destroyConnectDeclaration(declaration->connect);
				break;
		}
		free(declaration);
		destroyDeclaration(next);
	}
}

void destroyApp(App * app) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (app != NULL) {
		free(app->name);
		destroyDeclaration(app->declarations);
		free(app);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyApp(program->app);
		free(program);
	}
}
