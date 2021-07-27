// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InstanceLevelCollisionCommands.h"

#define LOCTEXT_NAMESPACE "InstanceLevelCollisionModule"

void InstanceLevelCollisionCommands::RegisterCommands()
{
	UI_COMMAND(InstanceLevelCollisionWidget, "Generate Collision", "Generate Collision Widget", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE