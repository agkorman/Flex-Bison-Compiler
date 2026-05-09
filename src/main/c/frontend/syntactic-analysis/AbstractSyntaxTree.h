#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

typedef enum RoleType RoleType;
typedef enum DeclarationType DeclarationType;

typedef struct ServiceDeclaration ServiceDeclaration;
typedef struct ExposeDeclaration ExposeDeclaration;
typedef struct ConnectDeclaration ConnectDeclaration;
typedef struct Declaration Declaration;
typedef struct App App;
typedef struct Program Program;

enum RoleType {
	FRONTEND_ROLE,
	API_ROLE,
	DATABASE_ROLE,
	CACHE_ROLE
};

enum DeclarationType {
	SERVICE_DECLARATION,
	EXPOSE_DECLARATION,
	CONNECT_DECLARATION
};

struct ServiceDeclaration {
	RoleType role;
	char * name;
	char * image;
};

struct ExposeDeclaration {
	char * serviceName;
	int port;
};

struct ConnectDeclaration {
	char * from;
	char * to;
};

struct Declaration {
	union {
		ServiceDeclaration * service;
		ExposeDeclaration * expose;
		ConnectDeclaration * connect;
	};
	DeclarationType type;
	Declaration * next;
};

struct App {
	char * name;
	Declaration * declarations;
};

struct Program {
	App * app;
};

void destroyServiceDeclaration(ServiceDeclaration * service);
void destroyExposeDeclaration(ExposeDeclaration * expose);
void destroyConnectDeclaration(ConnectDeclaration * connect);
void destroyDeclaration(Declaration * declaration);
void destroyApp(App * app);
void destroyProgram(Program * program);

#endif
