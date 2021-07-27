// Copyright Epic Games, Inc. All Rights Reserved.

#include "MovementAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h" // for FGameplayAbilityActorInfo, FGameplayModifierEvaluatedData
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMovementAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMoveSpeedAttribute())
	{
		if (ACharacter* TargetCharacter = GetTypedOuter<ACharacter>())
		{
			if (UCharacterMovementComponent* MovementComponent = TargetCharacter->GetCharacterMovement())
			{
				MovementComponent->MaxWalkSpeed = NewValue;
			}
		}
	}
}
