// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoundscapeSubsystem.h"
#include "SoundscapeSettings.h"
#include "SoundscapePalette.h"
#include "AudioDevice.h"

void USoundscapeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Get Subsystem World
	UWorld* World = GetWorld();

	// Initialize AudioDeviceID
	AudioDeviceID = INDEX_NONE;

	// Get world
	if (World)
	{
		// Get Audio Device Handle
		FAudioDevice* AudioDevice = World->GetAudioDeviceRaw();

		if (AudioDevice)
		{
			AudioDeviceID = AudioDevice->DeviceID;
		}

	}

	// Get plugin settings on Subsystem initialization
	if (const USoundscapeSettings* ProjectSettings = GetDefault<USoundscapeSettings>())
	{
		if (ProjectSettings->SoundscapePaletteCollection.IsEmpty() == false)
		{
			FSoundscapePaletteCollection PaletteCollectionToAdd;

			PaletteCollectionToAdd.SoundscapePaletteCollection = ProjectSettings->SoundscapePaletteCollection;

			AddPaletteCollection(FName("GlobalPaletteCollection"), PaletteCollectionToAdd);			
		}

		bDebugMode = ProjectSettings->bDebugDraw;
	}
}

void USoundscapeSubsystem::Deinitialize()
{

}

bool USoundscapeSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Check with parent first
	if (Super::ShouldCreateSubsystem(Outer) == false)
	{
		return false;
	}

	// Get World
	if (UWorld* OuterWorld = Outer->GetWorld())
	{
		if (FAudioDevice* OuterAudioDevice = OuterWorld->GetAudioDeviceRaw())
		{
			// If we do have an audio device, we can create this subsystem.
			return true;
		}
	}

	// If we do not have an audio device, we do not need to create this subsystem.
	return false;
}

void USoundscapeSubsystem::SetState(FGameplayTag SoundscapeState)
{
	if (SoundscapeState.IsValid())
	{
		// Add new state to the container
		SubsystemState.AddLeafTag(SoundscapeState);

		UpdateState();
	}
}

void USoundscapeSubsystem::ClearState(FGameplayTag SoundscapeState)
{
	if (SoundscapeState.IsValid())
	{
		// Add new state to the container
		SubsystemState.RemoveTag(SoundscapeState);

		UpdateState();
	}
}

bool USoundscapeSubsystem::AddPaletteCollection(FName PaletteCollectionName, FSoundscapePaletteCollection PaletteCollection)
{
	if (UnloadedPaletteCollections.Contains(PaletteCollectionName) == false)
	{
		UnloadedPaletteCollections.Add(PaletteCollectionName, PaletteCollection);

		bool bLoaded = LoadPaletteCollection(PaletteCollectionName);

		UpdateState();

		return bLoaded;
	}

	return false;
}

bool USoundscapeSubsystem::RemovePaletteCollection(FName PaletteCollectionName)
{
	if (UnloadedPaletteCollections.Find(PaletteCollectionName))
	{
		if (UnloadPaletteCollection(PaletteCollectionName))
		{
			// Unloaded, now remove
			UnloadedPaletteCollections.Remove(PaletteCollectionName);

			UpdateState();

			return true;
		}
	}

	return false;
}

bool USoundscapeSubsystem::LoadPaletteCollection(FName PaletteCollectionName)
{
	if (UnloadedPaletteCollections.Contains(PaletteCollectionName))
	{
		FSoundscapePaletteCollection PaletteCollectionToLoad = UnloadedPaletteCollections.FindRef(PaletteCollectionName);

		for (FSoftObjectPath& ObjPath : PaletteCollectionToLoad.SoundscapePaletteCollection)
		{
			if (UObject* PalettePath = ObjPath.TryLoad())
			{
				USoundscapePalette* SoundscapePalette = Cast<USoundscapePalette>(PalettePath);

				if (SoundscapePalette)
				{
					// If palette is valid, add it to the Subsystem Collection
					LoadedPaletteCollectionSet.Add(SoundscapePalette);
				}
			}
		}

		return true;
	}

	return false;
}

bool USoundscapeSubsystem::UnloadPaletteCollection(FName PaletteCollectionName)
{
	if (UnloadedPaletteCollections.Find(PaletteCollectionName))
	{
		// Get unloaded set to remove from list
		FSoundscapePaletteCollection PaletteCollectionToUnload = UnloadedPaletteCollections.FindRef(PaletteCollectionName);
		TSet<FSoftObjectPath> PaletteCollectionSetToUnload;
		PaletteCollectionSetToUnload.Append(PaletteCollectionToUnload.SoundscapePaletteCollection);
		TSet<FSoftObjectPath> PaletteCollectionSetToKeep;

		// Go through all unloaded collections and construct a unique list of Soft Object Paths
		for (TPair<FName, FSoundscapePaletteCollection>& UnloadedCollectionName : UnloadedPaletteCollections)
		{
			if (UnloadedCollectionName.Key != PaletteCollectionName)
			{
				PaletteCollectionSetToKeep.Append(UnloadedCollectionName.Value.SoundscapePaletteCollection.Intersect(PaletteCollectionSetToUnload));
			}
		}

		// Find the difference between what we want to retain and what we want to unload
		PaletteCollectionSetToUnload = PaletteCollectionSetToUnload.Difference(PaletteCollectionSetToKeep);

		// Temp set of palettes to remove and stop
		TSet<USoundscapePalette*> PalettesToUnloadStopAndRemove;

		// Go through collection, get their loaded pointers, add to temp removal set
		for (FSoftObjectPath& ObjPath : PaletteCollectionSetToUnload)
		{
			if (UObject* PalettePath = ObjPath.TryLoad())
			{
				USoundscapePalette* SoundscapePalette = Cast<USoundscapePalette>(PalettePath);

				if (SoundscapePalette)
				{
					// If palette is valid, add it to the Subsystem Collection
					PalettesToUnloadStopAndRemove.Add(SoundscapePalette);
				}
			}
		}

		// Stop and remove from Active list
		for (USoundscapePalette* PaletteKeyToStop : PalettesToUnloadStopAndRemove)
		{
			if (PaletteKeyToStop)
			{
				if (UActiveSoundscapePalette* ActiveSoundscapePalette = ActivePalettes.FindRef(PaletteKeyToStop))
				{
					ActiveSoundscapePalette->Stop();

					ActivePalettes.Remove(PaletteKeyToStop);
				}
			}
		}

		// Remove from main list of loaded palettes
		LoadedPaletteCollectionSet = LoadedPaletteCollectionSet.Difference(PalettesToUnloadStopAndRemove);

		return true;
	}

	return false;
}

void USoundscapeSubsystem::UpdateState()
{
	// Palettes the keep
	TMap<USoundscapePalette*, UActiveSoundscapePalette*> ActivePalettesToKeep;

	// Remove tags that don't match
	for (const TPair<USoundscapePalette*, UActiveSoundscapePalette*> ActiveSoundscapePalettePair : ActivePalettes)
	{
		USoundscapePalette* SoundscapePalette = ActiveSoundscapePalettePair.Key;
		UActiveSoundscapePalette* ActiveSoundscapePalette = ActiveSoundscapePalettePair.Value;

		if (SoundscapePalette && ActiveSoundscapePalette)
		{
			if (SoundscapePalette->SoundscapePalettePlaybackConditions.Matches(SubsystemState))
			{
				ActivePalettesToKeep.Add(SoundscapePalette, ActiveSoundscapePalette);
			}
			else
			{
				ActiveSoundscapePalette->Stop();
			}
		}
	}

	// Empty active palettes an append ones to keep
	ActivePalettes.Empty();
	ActivePalettes.Append(ActivePalettesToKeep);

	// Add matching palettes
	for (auto PaletteIterator = LoadedPaletteCollectionSet.CreateConstIterator(); PaletteIterator; ++PaletteIterator)
	{
		if (USoundscapePalette* SoundscapePalette = *PaletteIterator)
		{
			// Evaluate if the Soundscape Palette matches
			if (SoundscapePalette->SoundscapePalettePlaybackConditions.Matches(SubsystemState) && ActivePalettes.Contains(SoundscapePalette) == false)
			{
				UWorld* World = GetWorld();

				if (World)
				{
					// Create a new ActiveSoundscapeColor
					UActiveSoundscapePalette* ActiveSoundscapePalette = NewObject<UActiveSoundscapePalette>(World);

					//
					ActiveSoundscapePalette->InitializeSettings(World, SoundscapePalette);

					ActiveSoundscapePalette->Play();

					ActivePalettes.Add(SoundscapePalette, ActiveSoundscapePalette);

				}
			}
		}
	}
}
