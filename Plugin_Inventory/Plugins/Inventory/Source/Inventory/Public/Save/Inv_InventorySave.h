// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_InventorySave.generated.h"

/**
 * 
 */

USTRUCT()
struct FItemSaveData
{
	GENERATED_BODY()


	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY()
	FIntPoint ItemLocation = FIntPoint();

	UPROPERTY()
	FInv_ItemManifest ItemManifest = FInv_ItemManifest();

	UPROPERTY()
	bool bEquipped = false;
};
UCLASS()
class INVENTORY_API UInv_InventorySave : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString("Default Name");

	TArray<FItemSaveData> InventoryItems;

};
