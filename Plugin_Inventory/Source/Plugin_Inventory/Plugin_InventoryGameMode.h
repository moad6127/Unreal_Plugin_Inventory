// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Plugin_InventoryGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class APlugin_InventoryGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	APlugin_InventoryGameMode();
};



