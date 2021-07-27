// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_SoundscapePalette.h"
#include "SoundscapePalette.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_SoundscapePalette::GetSupportedClass() const
{
	return USoundscapePalette::StaticClass();
}

const TArray<FText>& FAssetTypeActions_SoundscapePalette::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("AssetSoundscapeSubMenu", "Soundscape")
	};
	return SubMenus;
}


#undef LOCTEXT_NAMESPACE
