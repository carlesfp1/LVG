// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoundscapeColor.h"
#include "UObject/WeakObjectPtr.h"
#include "Components/AudioComponent.h"
#include "AudioDevice.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "SoundscapeSubsystem.h"

USoundscapeColor::USoundscapeColor()
	: VolumeBase(1.0f)
	, PitchBase(1.0f)
{
}

void USoundscapeColor::PostLoad()
{
	Super::PostLoad();
}

void USoundscapeColor::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

#if WITH_EDITORONLY_DATA
void USoundscapeColor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Broadcast changes to the properties so instances can update
	OnSoundscapeColorParameterChanges.Broadcast(this);

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UActiveSoundscapeColorVoice::BeginDestroy()
{
	if (UWorld* World = GetWorld())
	{
		// Clear out all timers
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	Super::BeginDestroy();
}

void UActiveSoundscapeColorVoice::StopLimitedDurationVoice(float FadeOutTime)
{
	if (AudioComponent)
	{
		AudioComponent->FadeOut(FadeOutTime, 0.0f);
	}
}

void UActiveSoundscapeColor::BeginDestroy()
{
	if (UWorld* World = GetWorld())
	{
		// Clear out all timers
		World->GetTimerManager().ClearTimer(UpdateTimer);
	}

	Super::BeginDestroy();
}

void UActiveSoundscapeColor::SetParameterValues(const USoundscapeColor* SoundscapeColor)
{
	if (SoundscapeColor)
	{
		Sound = SoundscapeColor->Sound;

		VolumeBase = SoundscapeColor->VolumeBase;
		PitchBase = SoundscapeColor->PitchBase;

		ModulationBehavior = SoundscapeColor->ModulationBehavior;
		PlaybackBehavior = SoundscapeColor->PlaybackBehavior;
		SpawnBehavior = SoundscapeColor->SpawnBehavior;
	}
}

#if WITH_EDITOR
void UActiveSoundscapeColor::BindToParameterChangeDelegate(USoundscapeColor* SoundscapeColor)
{

#if WITH_EDITORONLY_DATA
	SoundscapeParameterChangeDelegate.BindUFunction(this, TEXT("OnSoundscapeColorParameterChange"));
	SoundscapeColor->OnSoundscapeColorParameterChanges.Add(SoundscapeParameterChangeDelegate);
#endif
}
#endif

#if WITH_EDITOR
void UActiveSoundscapeColor::OnSoundscapeColorParameterChange(const USoundscapeColor* SoundscapeColor)
{
	SetParameterValues(SoundscapeColor);
}
#endif

void UActiveSoundscapeColor::PlayNative()
{
	if (bIsPlaying)
	{
		// Early out if we're already playing
		return;
	}

	// Call internal play
	StartPlaying();

}

void UActiveSoundscapeColor::StopNative()
{
	if (bIsPlaying == false)
	{
		// Early out if we've already stopped
		return;
	}

	// Call internal stop
	StopPlaying();

}

void UActiveSoundscapeColor::Play(float ColorVolume, float ColorPitch, float ColorFadeInTime)
{
	if (bIsPlaying)
	{
		// Early out if we're already playing
		return;
	}

	// Cache play values
	VolumeMod = ColorVolume;
	PitchMod = ColorPitch;
	FadeInMin = ColorFadeInTime;

	// Call internal play
	StartPlaying();
}

void UActiveSoundscapeColor::Stop(float ColorFadeOutTime)
{
	if (bIsPlaying == false)
	{
		// Early out if we've already stopped
		return;
	}

	// Cache stop values
	FadeOutMin = ColorFadeOutTime;

	// Call internal stop
	StopPlaying();
}

bool UActiveSoundscapeColor::IsPlaying()
{
	return bIsPlaying;
}

void UActiveSoundscapeColor::UpdateSoundscapeColor()
{
	if (bIsPlaying)
	{
		Update();
	}
}

void UActiveSoundscapeColor::StartPlaying()
{
	// Update state
	bIsPlaying = true;
	bFirstSpawn = true;

	// Set up for first Timer delay
	float FirstDelayTime = 0.0001f;

	if (SpawnBehavior.bDelayFirstSpawn)
	{
		// If we delay the first spawn, then 
		FirstDelayTime = FMath::FRandRange(SpawnBehavior.MinFirstSpawnDelay, SpawnBehavior.MaxFirstSpawnDelay);
	}
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(UpdateTimer, this, &UActiveSoundscapeColor::UpdateSoundscapeColor, FirstDelayTime);
	}
}

void UActiveSoundscapeColor::StopPlaying()
{
	// Stop with appropriate fade time
	for (UActiveSoundscapeColorVoice* SoundscapeColorVoice : SoundscapeColorVoicePool)
	{
		if (SoundscapeColorVoice)
		{

			if (UAudioComponent* VoiceAudioComponent = SoundscapeColorVoice->AudioComponent)
			{
				if (EAudioComponentPlayState::Stopped != VoiceAudioComponent->GetPlayState())
				{
					VoiceAudioComponent->FadeOut(FadeOutMin, 0.0f);
				}

				if (UWorld* World = GetWorld())
				{
					// Clear out all timers
					World->GetTimerManager().ClearTimer(SoundscapeColorVoice->TimerHandle);
				}
				
			}
		}
	}

	bIsPlaying = false;
}

void UActiveSoundscapeColor::Update()
{
	UWorld* World = GetWorld();
	bool bNeedToSpawnSound = false;
	bool bVoiceInPoolFree = false;
	int32 ActiveVoicesPlaying = 0;

	// Free up old voices
	for (UActiveSoundscapeColorVoice* SoundscapeColorVoice : SoundscapeColorVoicePool)
	{
		if (SoundscapeColorVoice)
		{
			
			if (UAudioComponent* VoiceAudioComponent = SoundscapeColorVoice->AudioComponent)
			{

				if (EAudioComponentPlayState::Stopped == VoiceAudioComponent->GetPlayState())
				{
					// Voice is not in use, mark it as free to use
					SoundscapeColorVoice->bFree = true;

					// Flag that a voice is available
					bVoiceInPoolFree = true;
				}
				else
				{
					// Voice is in use, increment active voice count
					++ActiveVoicesPlaying;
				}
			}
		}
	}

	// Determine if conditions are right to play a new sound
	if (ActiveVoicesPlaying < SpawnBehavior.MaxNumberOfSpawnedElements)
	{
		bNeedToSpawnSound = true;
	}

	// Set up timer if needed
	if (SpawnBehavior.bContinuouslyRespawn)
	{
		// Set random range spawn time, with a minimum value of 0.0001f (which is basically next frame)
		float SpawnDelayTime = FMath::Max(0.0001f, FMath::FRandRange(SpawnBehavior.MinSpawnDelay, SpawnBehavior.MaxSpawnDelay));

		// Set off timer again
		if (World)
		{
			World->GetTimerManager().SetTimer(UpdateTimer, this, &UActiveSoundscapeColor::UpdateSoundscapeColor, SpawnDelayTime);
		}
	}

	// Play Sound if needed and valid sound available
	if (Sound && bNeedToSpawnSound && World)
	{
		// Get the new sound's volume
		float NewRandVolume = ModulationBehavior.bRandomizeVolume ? FMath::FRandRange(ModulationBehavior.VolumeMin, ModulationBehavior.VolumeMax) : 1.0f;
		float NewSoundVolume = VolumeMod * VolumeBase * NewRandVolume;

		// Get the new sound's pitch
		float NewRandPitch = ModulationBehavior.bRandomizePitch ? FMath::FRandRange(ModulationBehavior.PitchMin, ModulationBehavior.PitchMax) : 1.0f;
		float NewSoundPitch = PitchMod * PitchBase * NewRandPitch;

		// Get the new sound's fade in time
		float NewRandFadeIn = ModulationBehavior.bFadeVolume ? FMath::FRandRange(ModulationBehavior.MinFadeInTime, ModulationBehavior.MaxFadeInTime) : 0.0f;
		float NewSoundFadeIn = bFirstSpawn ? FMath::Max(FadeInMin, NewRandFadeIn) : NewRandFadeIn;
		NewSoundFadeIn = (bFirstSpawn && ModulationBehavior.bOnlyFadeInOnRetrigger) ? 0.0f : NewSoundFadeIn;

		// Get the new sound's start time
		float NewSoundStartTime = PlaybackBehavior.bRandomizeStartingSeekTime ? FMath::FRandRange(0.0f, Sound->Duration) : 0.0f;

		FVector NewSoundSpawnLocation;
		FVector ListenerLocation;
		FVector ListenerForward;
		FVector ListenerUp;

		if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
		{
			// Get available Listener Proxies
			TArray<FListenerProxy>& ListenerProxies = AudioDevice->ListenerProxies;

			// TODO: Handle Split Screen
			if (ListenerProxies.Num())
			{
				// Get Location and Direction from Listener
				FTransform& ListenerTransform = ListenerProxies[0].Transform;
				ListenerLocation = ListenerTransform.GetLocation();
				ListenerForward = ListenerTransform.GetRotation().GetForwardVector();
				ListenerUp = ListenerTransform.GetRotation().GetUpVector();
			}
		}

		// Get random distance based on range
		float NewSpawnDistance = FMath::RandRange(SpawnBehavior.MinSpawnDistance, SpawnBehavior.MaxSpawnDistance);

		// Get angle from min/max range then
		float NewSpawnAngle = FMath::RandRange(SpawnBehavior.MinSpawnAngle, SpawnBehavior.MaxSpawnAngle);

		// Sometimes Left Sometimes Right
		if (FMath::RandBool())
		{
			NewSpawnAngle = NewSpawnAngle * (-1.0f);
		}

		// Get X Y Vector
		NewSoundSpawnLocation = ListenerForward.RotateAngleAxis(NewSpawnAngle, ListenerUp);

		// Random Z Vector
		NewSoundSpawnLocation.Z = FMath::RandRange(-1.0f, 1.0f);

		// Scale Vector and add to Listener Location
		NewSoundSpawnLocation = (NewSoundSpawnLocation * NewSpawnDistance) + ListenerLocation;

		// Clamp the Z Altitude
		NewSoundSpawnLocation.Z = SpawnBehavior.bClampRelativeHeight ? FMath::Clamp(NewSoundSpawnLocation.Z, ListenerLocation.Z + SpawnBehavior.MinSpawnHeightClamp, ListenerLocation.Z + SpawnBehavior.MaxSpawnHeightClamp) : NewSoundSpawnLocation.Z;

		FRotator NewSoundRotation = FRotator(0.0f, 0.0f, 0.0f);

		if (SpawnBehavior.bRotateSoundSource)
		{
			NewSoundRotation.Yaw = FMath::Max(0.0f, FMath::FRandRange(SpawnBehavior.MinAzimuthalRotationAngle, SpawnBehavior.MaxAzimuthalRotationAngle));
			NewSoundRotation.Pitch = FMath::Max(0.0f, FMath::FRandRange(SpawnBehavior.MinAltitudinalRotationAngle, SpawnBehavior.MaxAltitudinalRotationAngle));
		}

		// Set up Voice and Component
		UActiveSoundscapeColorVoice* NewActiveSoundscapeColorVoice = nullptr;
		UAudioComponent* NewAudioComponent = nullptr;

		// If there's a free voice in the pool, get first available voice
		if (bVoiceInPoolFree)
		{
			for (UActiveSoundscapeColorVoice* ActiveSoundscapeColorVoice : SoundscapeColorVoicePool)
			{
				if (ActiveSoundscapeColorVoice)
				{
					// Get first free voice
					if (ActiveSoundscapeColorVoice->bFree)
					{
						// Cache pointer to this voice
						NewActiveSoundscapeColorVoice = ActiveSoundscapeColorVoice;

						break;
					}

				}
			}
		}
		
		// If the ActiveSoundscapeColorVoice is still null, create a new one
		if (NewActiveSoundscapeColorVoice == nullptr)
		{
			// Create new voice
			NewActiveSoundscapeColorVoice = NewObject<UActiveSoundscapeColorVoice>(World);

			// Add new voice to the pool
			SoundscapeColorVoicePool.Add(NewActiveSoundscapeColorVoice);
		}

		// If the Audio Component invalid, create a new one
		if (NewActiveSoundscapeColorVoice->AudioComponent == nullptr)
		{
			NewActiveSoundscapeColorVoice->AudioComponent = NewObject<UAudioComponent>(World);
		}

		// Cache Audio Component ptr
		NewAudioComponent = NewActiveSoundscapeColorVoice->AudioComponent;

		// Set World location of Audio Component
		NewAudioComponent->SetWorldLocationAndRotation(NewSoundSpawnLocation, NewSoundRotation.Quaternion());

		// Set relevant Audio Component values before playing
		NewAudioComponent->SetPitchMultiplier(NewSoundPitch);
		NewAudioComponent->SetSound(Sound);

		NewAudioComponent->bAutoDestroy = false;

		// Play the sound
		NewAudioComponent->FadeIn(NewSoundFadeIn, NewSoundVolume, NewSoundStartTime);

		// Voice is no longer free
		NewActiveSoundscapeColorVoice->bFree = false;

		float DebugDrawDuration = Sound->Duration;

		// Handle limited duration sounds
		if (PlaybackBehavior.bLimitPlaybackDuration)
		{
			float FadeOutDuration = FMath::Max(0.0001f, FMath::FRandRange(ModulationBehavior.MinFadeOutTime, ModulationBehavior.MaxFadeOutTime));
			float LimitedDuration = FMath::Max(0.0001f, FMath::FRandRange(PlaybackBehavior.MinPlaybackDuration, PlaybackBehavior.MaxPlaybackDuration));
			FTimerDelegate TimerDelegate;

			DebugDrawDuration = LimitedDuration;

			TimerDelegate.BindUFunction(NewActiveSoundscapeColorVoice, FName("StopLimitedDurationVoice"), FadeOutDuration);
			GetWorld()->GetTimerManager().SetTimer(NewActiveSoundscapeColorVoice->TimerHandle, TimerDelegate, LimitedDuration, false);
		}

		// Draw debug spheres if in debug mode
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (USoundscapeSubsystem* SoundscapeSubsystem = GameInstance->GetSubsystem<USoundscapeSubsystem>())
			{
				if (SoundscapeSubsystem->bDebugMode)
				{
					DebugDrawDuration = DebugDrawDuration / FMath::Max(NewSoundPitch, 0.0001f);
					DrawDebugSphere(World, NewSoundSpawnLocation, 20.0f, 10, FColor::Green, false, DebugDrawDuration);

				}
			}
		}

	}

	// No longer the first time
	bFirstSpawn = false;
}