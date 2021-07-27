// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CurveUtilLibrary.generated.h"

struct FRuntimeFloatCurve;

UCLASS()
class UCurveUtilLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Curve")
	static float EvaluateCurve(UPARAM(ref) const FRuntimeFloatCurve& RuntimeCurve, float InTime);

	UFUNCTION(BlueprintPure, Category = "Curve")
	static float FInterpCubic(float A, float B, float Alpha);
};
