// Copyright Epic Games, Inc. All Rights Reserved.


#include "UproarChaosBreakEventSettingsFactory.h"
#include "UproarChaosBreakEventSettings.h"

UUproarChaosBreakEventSettingsFactory::UUproarChaosBreakEventSettingsFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UUproarChaosBreakEventSettings::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UUproarChaosBreakEventSettingsFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UUproarChaosBreakEventSettings>(InParent, Name, Flags);

}
