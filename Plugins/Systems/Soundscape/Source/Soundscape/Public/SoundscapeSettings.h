// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPath.h"
#include "SoundscapeSettings.generated.h"

/**
 * 
 */
UCLASS(config = Soundscape, defaultconfig, meta = (DisplayName = "Soundscape"))
class SOUNDSCAPE_API USoundscapeSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:

	// Soundscape Palette Collection
	UPROPERTY(config, EditAnywhere, meta = (AllowedClasses = "SoundscapePalette"))
	TSet<FSoftObjectPath> SoundscapePaletteCollection;

	UPROPERTY(config, EditAnywhere)
	bool bDebugDraw = false;

public:

	// Beginning of UDeveloperSettings Interface
	virtual FName GetCategoryName() const override { return FName(TEXT("Game")); }
#if WITH_EDITOR
	virtual FText GetSectionText() const override { return NSLOCTEXT("SoundscapePlugin", "SoundscapeSettingsSection", "Soundscape"); };
#endif
	// End of UDeveloperSettings Interface


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
