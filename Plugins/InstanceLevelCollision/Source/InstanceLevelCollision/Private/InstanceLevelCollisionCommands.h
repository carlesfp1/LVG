// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EditorStyleSet.h"


class InstanceLevelCollisionCommands : public TCommands<InstanceLevelCollisionCommands>
{
public:

	InstanceLevelCollisionCommands()
		: TCommands<InstanceLevelCollisionCommands>
		(
			TEXT("Collision Tool"),
			NSLOCTEXT("CollisionTool", "Collision Tool", "CollisionTool plugin"),
			NAME_None,
			FEditorStyle::GetStyleSetName()
			) {}

	virtual void RegisterCommands() override;

public:

	TSharedPtr<FUICommandInfo> InstanceLevelCollisionWidget;
};