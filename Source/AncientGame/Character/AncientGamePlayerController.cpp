// Copyright Epic Games, Inc. All Rights Reserved.

#include "AncientGamePlayerController.h"

void AAncientGamePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	OnRecievedPlayer.Broadcast(Player);
}
