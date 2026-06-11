#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

typedef enum RoleType RoleType;
typedef enum DeclarationType DeclarationType;
typedef enum MountSourceType MountSourceType;
typedef enum AppItemType AppItemType;

typedef struct ServiceDeclaration ServiceDeclaration;
typedef struct ExposeDeclaration ExposeDeclaration;
typedef struct ConnectDeclaration ConnectDeclaration;
typedef struct VolumeDeclaration VolumeDeclaration;
typedef struct MountDeclaration MountDeclaration;
typedef struct Declaration Declaration;
typedef struct Network Network;
typedef struct AppItem AppItem;
typedef struct App App;
typedef struct Program Program;

enum RoleType {
	PROXY_ROLE,
	SERVICE_ROLE,
	STATIC_ROLE,
	DATABASE_ROLE,
	CACHE_ROLE
};

enum DeclarationType {
	SERVICE_DECLARATION,
	EXPOSE_DECLARATION,
	CONNECT_DECLARATION,
	VOLUME_DECLARATION,
	MOUNT_DECLARATION
};

enum MountSourceType {
	VOLUME_SOURCE,
	HOST_PATH_SOURCE
};

enum AppItemType {
	NETWORK_ITEM,
	NETWORK_CONNECT_ITEM
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

struct VolumeDeclaration {
	char * name;
};

struct MountDeclaration {
	MountSourceType sourceType;
	char * source;
	char * serviceName;
	char * containerPath;
};

struct Declaration {
	union {
		ServiceDeclaration * service;
		ExposeDeclaration * expose;
		ConnectDeclaration * connect;
		VolumeDeclaration * volume;
		MountDeclaration * mount;
	};
	DeclarationType type;
	Declaration * next;
};

struct Network {
	char * name;
	Declaration * declarations;
};

struct AppItem {
	union {
		Network * network;
		ConnectDeclaration * connect;
	};
	AppItemType type;
	AppItem * next;
};

struct App {
	char * name;
	AppItem * items;
};

struct Program {
	App * app;
};

void destroyServiceDeclaration(ServiceDeclaration * service);
void destroyExposeDeclaration(ExposeDeclaration * expose);
void destroyConnectDeclaration(ConnectDeclaration * connect);
void destroyVolumeDeclaration(VolumeDeclaration * volume);
void destroyMountDeclaration(MountDeclaration * mount);
void destroyDeclaration(Declaration * declaration);
void destroyNetwork(Network * network);
void destroyAppItem(AppItem * item);
void destroyApp(App * app);
void destroyProgram(Program * program);

#endif
