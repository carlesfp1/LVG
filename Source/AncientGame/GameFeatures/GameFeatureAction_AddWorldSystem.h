// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddWorldSystem.generated.h"

/** Base class for world system objects. */
UCLASS(Abstract, Blueprintable)
class UGameFeatureWorldSystem : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Initialize(const UObject* WorldContextObject);
};

/** Record for the game feature data. Specifies what type of system to instantiate. */
USTRUCT()
struct FGameFeatureWorldSystemEntry
{
	GENERATED_BODY()

	// The world to spawn this system for (can be left blank, in which case we'll spawn this for all worlds)
	UPROPERTY(EditAnywhere, Category="World System")
	TSoftObjectPtr<UWorld> TargetWorld;

	// The type to instantiate
	UPROPERTY(EditAnywhere, Category = "World System")
	TSubclassOf<UGameFeatureWorldSystem> SystemType;
};	

/**
 * GameFeature action which allows for spawning singleton style world objects for a
 * specific feature (allows for users to define those system objects in Blueprints).
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add World System"))
class UGameFeatureAction_AddWorldSystem final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
	//~ End UObject interface

	UPROPERTY(EditAnywhere, Category="World System")
	TArray<FGameFeatureWorldSystemEntry> WorldSystemsList;

	UFUNCTION(BlueprintCallable, Category = "Game Features|World Systems", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "SystemType"))
	static UGameFeatureWorldSystem* FindGameFeatureWorldSystemOfType(TSubclassOf<UGameFeatureWorldSystem> SystemType, UObject* WorldContextObject);

private:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	void Reset();
};

/** 
 * C++ WorldSubsystem to manage requested system instances 
 * (ref counts requests to account for multiple feature requesting the same system). 
 */
UCLASS()
class UGameFeatureWorldSystemManager : public UWorldSubsystem
{
	GENERATED_BODY()

	//~ Begin UWorldSubsystem interface
	virtual void PostInitialize() override;
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	//~ End UWorldSubsystem interface

private:
	friend class UGameFeatureAction_AddWorldSystem;

	UGameFeatureWorldSystem* RequestSystemOfType(TSubclassOf<UGameFeatureWorldSystem> SystemType);
	void ReleaseRequestForSystemOfType(TSubclassOf<UGameFeatureWorldSystem> SystemType);

	UPROPERTY(transient)
	TMap<UGameFeatureWorldSystem*, int32> SystemInstances;

	bool bIsInitialized = false;
};