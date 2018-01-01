// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheLastBastion.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, TheLastBastion, "TheLastBastion" );
 

//General Log
DEFINE_LOG_CATEGORY(LogTNB);

//Logging during game startup
DEFINE_LOG_CATEGORY(LogTNBInit);

//Logging for your AI system
DEFINE_LOG_CATEGORY(LogTNBAI);