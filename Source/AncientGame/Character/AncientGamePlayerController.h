// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularPlayerController.h"
#include "AncientGamePlayerController.generated.h"

UCLASS()
class AAncientGamePlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	//~ Begin APlayerController interface
	virtual void ReceivedPlayer() override;
	//~ End APlayerController interface

	FUNC_DECLARE_EVENT(AAncientGamePlayerController, FOnRecievedPlayerEvent, void, UPlayer*);
	FOnRecievedPlayerEvent OnRecievedPlayer;
};
