// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "AncientGameModeBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ANCIENTGAME_API AAncientGameModeBase : public AModularGameModeBase
{
	GENERATED_BODY()
public:

	//~ Begin AGameModeBase Interface
	virtual void StartPlay() override;
	//~ End AGameModeBase Interface

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BeginDarkWorldTransition(FVector QuerySourceLocation);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PrefetchDarkWorld(FVector QuerySourceLocation);

	UFUNCTION(BlueprintCallable)
	void ToggleGameFeaturePlugin(FString GameFeaturePluginName, bool bEnable);

};
