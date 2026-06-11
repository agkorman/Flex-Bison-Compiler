#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Program * AppProgramSemanticAction(App * app) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->app = app;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

App * AppSemanticAction(char * name, AppItem * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	App * app = calloc(1, sizeof(App));
	app->name = name;
	app->items = items;
	return app;
}

AppItem * SingleAppItemSemanticAction(AppItem * item) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return item;
}

AppItem * AppendAppItemSemanticAction(AppItem * list, AppItem * item) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AppItem * current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = item;
	return list;
}

AppItem * NetworkAppItemSemanticAction(Network * network) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AppItem * item = calloc(1, sizeof(AppItem));
	item->network = network;
	item->type = NETWORK_ITEM;
	item->next = NULL;
	return item;
}

AppItem * NetworkConnectAppItemSemanticAction(char * from, char * to) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AppItem * item = calloc(1, sizeof(AppItem));
	ConnectDeclaration * connect = calloc(1, sizeof(ConnectDeclaration));
	connect->from = from;
	connect->to = to;
	item->connect = connect;
	item->type = NETWORK_CONNECT_ITEM;
	item->next = NULL;
	return item;
}

Network * NetworkSemanticAction(char * name, Declaration * declarations) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Network * network = calloc(1, sizeof(Network));
	network->name = name;
	network->declarations = declarations;
	return network;
}

Declaration * SingleDeclarationSemanticAction(Declaration * declaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return declaration;
}

Declaration * AppendDeclarationSemanticAction(Declaration * list, Declaration * declaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = declaration;
	return list;
}

Declaration * ServiceDeclarationSemanticAction(RoleType role, char * name, char * image) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	ServiceDeclaration * service = calloc(1, sizeof(ServiceDeclaration));
	service->role = role;
	service->name = name;
	service->image = image;
	declaration->service = service;
	declaration->type = SERVICE_DECLARATION;
	declaration->next = NULL;
	return declaration;
}

Declaration * ExposeDeclarationSemanticAction(char * serviceName, int port) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	ExposeDeclaration * expose = calloc(1, sizeof(ExposeDeclaration));
	expose->serviceName = serviceName;
	expose->port = port;
	declaration->expose = expose;
	declaration->type = EXPOSE_DECLARATION;
	declaration->next = NULL;
	return declaration;
}

Declaration * ConnectDeclarationSemanticAction(char * from, char * to) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	ConnectDeclaration * connect = calloc(1, sizeof(ConnectDeclaration));
	connect->from = from;
	connect->to = to;
	declaration->connect = connect;
	declaration->type = CONNECT_DECLARATION;
	declaration->next = NULL;
	return declaration;
}

Declaration * VolumeDeclarationSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	VolumeDeclaration * volume = calloc(1, sizeof(VolumeDeclaration));
	volume->name = name;
	declaration->volume = volume;
	declaration->type = VOLUME_DECLARATION;
	declaration->next = NULL;
	return declaration;
}

Declaration * MountDeclarationSemanticAction(MountSourceType sourceType, char * source, char * serviceName, char * containerPath) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	MountDeclaration * mount = calloc(1, sizeof(MountDeclaration));
	mount->sourceType = sourceType;
	mount->source = source;
	mount->serviceName = serviceName;
	mount->containerPath = containerPath;
	declaration->mount = mount;
	declaration->type = MOUNT_DECLARATION;
	declaration->next = NULL;
	return declaration;
}
