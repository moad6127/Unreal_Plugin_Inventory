// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_InventorySaveTypes.generated.h"

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
	int32 ItemIndex = 0;

	UPROPERTY()
	FInv_ItemManifest ItemManifest = FInv_ItemManifest();

	UPROPERTY()
	bool bEquipped = false;
};

USTRUCT()
struct FInventorySaveData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FItemSaveData> InventoryItems;
};

