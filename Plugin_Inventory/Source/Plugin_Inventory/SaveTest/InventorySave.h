// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Save/Inv_InventorySaveTypes.h"
#include "InventorySave.generated.h"

/**
 * 
 */
UCLASS()
class PLUGIN_INVENTORY_API UInventorySave : public USaveGame
{
	GENERATED_BODY()
public:


	UPROPERTY()
	FInventorySaveData InventoryData;
};
