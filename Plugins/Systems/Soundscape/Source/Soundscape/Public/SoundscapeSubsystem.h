// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Audio.h"
#include "SoundscapeSubsystem.generated.h"

class USoundscapePalette;
class UActiveSoundscapePalette;


// Struct 
USTRUCT(BlueprintType)
struct SOUNDSCAPE_API FSoundscapePaletteCollection
{
	GENERATED_BODY()

		// Soundscape Palette Collection
		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowedClasses = "SoundscapePalette"))
		TSet<FSoftObjectPath> SoundscapePaletteCollection;
};

// Struct 
USTRUCT()
struct SOUNDSCAPE_API FSoundscapePaletteCollectionLoaded
{
	GENERATED_BODY()

	// Soundscape Palette Collection
	UPROPERTY(EditAnywhere)
	TSet<USoundscapePalette*> SoundscapePaletteCollection;
};

/**
 * 
 */
UCLASS()
class SOUNDSCAPE_API USoundscapeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// End USubsystem


	// Settings
public:

	UFUNCTION(BlueprintCallable, Category = "Soundscape")
	void SetState(FGameplayTag SoundscapeState);

	UFUNCTION(BlueprintCallable, Category = "Soundscape")
	void ClearState(FGameplayTag SoundscapeState);

	UFUNCTION(BlueprintCallable, Category = "Soundscape")
	bool AddPaletteCollection(FName PaletteCollectionName, FSoundscapePaletteCollection PaletteCollection);

	UFUNCTION(BlueprintCallable, Category = "Soundscape")
	bool RemovePaletteCollection(FName PaletteCollectionName);

	bool bDebugMode = false;

private:
	UPROPERTY()
	TSet<USoundscapePalette*> LoadedPaletteCollectionSet;

	UPROPERTY()
	TMap<FName, FSoundscapePaletteCollection> UnloadedPaletteCollections;

	FGameplayTagContainer SubsystemState;

	UPROPERTY()
	TMap<USoundscapePalette*, UActiveSoundscapePalette*> ActivePalettes;

	bool LoadPaletteCollection(FName PaletteCollectionName);

	bool UnloadPaletteCollection(FName PaletteCollectionName);

	void UpdateState();

private:

	Audio::FDeviceId AudioDeviceID;
};
