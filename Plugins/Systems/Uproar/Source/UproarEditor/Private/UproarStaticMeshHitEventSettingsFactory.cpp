// Copyright Epic Games, Inc. All Rights Reserved.


#include "UproarStaticMeshHitEventSettingsFactory.h"
#include "UproarStaticMeshHitEventSettings.h"

UUproarStaticMeshHitEventSettingsFactory::UUproarStaticMeshHitEventSettingsFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UUproarStaticMeshHitEventSettings::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UUproarStaticMeshHitEventSettingsFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UUproarStaticMeshHitEventSettings>(InParent, Name, Flags);

}

