// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/AncientGameCameraModeView.h"
#include "AncientGameCameraModeStack.generated.h"

class UAncientGameCameraMode;
class AActor;

/**
 * FAncientGameCameraModeStack
 *
 *	Stack used for blending camera modes.
 */
USTRUCT()
struct FAncientGameCameraModeStack
{
	GENERATED_BODY()

public:
	void PushCameraMode(UAncientGameCameraMode* CameraModeInstance, AActor* TargetActor);

	bool EvaluateStack(float DeltaTime, AActor* TargetActor, FAncientGameCameraModeView& OutCameraModeView);

protected:
	bool UpdateStack(float DeltaTime, AActor* TargetActor);
	void BlendStack(FAncientGameCameraModeView& OutCameraModeView) const;

protected:
	UPROPERTY()
	TArray<UAncientGameCameraMode*> CameraModeStack;
};
