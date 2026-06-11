#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

/**
 * Bison semantic actions.
 */

Program * AppProgramSemanticAction(App * app);
App * AppSemanticAction(char * name, AppItem * items);
AppItem * SingleAppItemSemanticAction(AppItem * item);
AppItem * AppendAppItemSemanticAction(AppItem * list, AppItem * item);
AppItem * NetworkAppItemSemanticAction(Network * network);
AppItem * NetworkConnectAppItemSemanticAction(char * from, char * to);
Network * NetworkSemanticAction(char * name, Declaration * declarations);
Declaration * SingleDeclarationSemanticAction(Declaration * declaration);
Declaration * AppendDeclarationSemanticAction(Declaration * list, Declaration * declaration);
Declaration * ServiceDeclarationSemanticAction(RoleType role, char * name, char * image);
Declaration * ExposeDeclarationSemanticAction(char * serviceName, int port);
Declaration * ConnectDeclarationSemanticAction(char * from, char * to);
Declaration * VolumeDeclarationSemanticAction(char * name);
Declaration * MountDeclarationSemanticAction(MountSourceType sourceType, char * source, char * serviceName, char * containerPath);

#endif
