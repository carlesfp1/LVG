// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/AncientGameModeBase.h"
#include "GameFeaturesSubsystem.h"
#include "Engine/Engine.h"

void AAncientGameModeBase::StartPlay()
{
	// Make sure level streaming is up to date before triggering NotifyMatchStarted
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());
	Super::StartPlay();
}

void AAncientGameModeBase::ToggleGameFeaturePlugin(FString GameFeaturePluginName, bool bEnable)
{
	FString PluginURL;
	
	UGameFeaturesSubsystem::Get().GetPluginURLForBuiltInPluginByName(GameFeaturePluginName, PluginURL);

	bEnable ? UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete()) : UGameFeaturesSubsystem::Get().UnloadGameFeaturePlugin(PluginURL);
}
