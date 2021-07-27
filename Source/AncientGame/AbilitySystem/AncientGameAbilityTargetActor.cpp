// Copyright Epic Games, Inc. All Rights Reserved.

#include "AncientGameAbilityTargetActor.h"
#include "AncientGame.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/BlueprintGeneratedClass.h"

void AAncientGameAbilityTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	
	RecieveStartTargeting(Ability);
}

bool AAncientGameAbilityTargetActor::IsConfirmTargetingAllowed()
{
	return K2_IsConfirmTargetingAllowed();
}

void AAncientGameAbilityTargetActor::ConfirmTargetingAndContinue()
{
	if (SourceActor && ensureAlways(ShouldProduceTargetData()))
	{
		auto ImplementedInBlueprint = [](const UFunction* Func) -> bool
		{
			return Func && ensure(Func->GetOuter())
				&& (Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass()) || Func->GetOuter()->IsA(UDynamicClass::StaticClass()));
		};
		UE_CLOG(
			!ImplementedInBlueprint(GetClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(AAncientGameAbilityTargetActor, MakeTargetData))),
			LogAncientGame,
			Error,
			TEXT("`%s` does not implement `MakeTargetData`. Invalid target data will be returned."),
			*GetClass()->GetName()
		);

		FGameplayAbilityTargetDataHandle Handle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

bool AAncientGameAbilityTargetActor::K2_IsConfirmTargetingAllowed_Implementation()
{
	return Super::IsConfirmTargetingAllowed();
}