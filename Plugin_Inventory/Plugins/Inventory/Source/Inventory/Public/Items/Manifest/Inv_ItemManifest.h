// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "Inv_ItemManifest.generated.h"

/*
* The Item Manifest contains all of the necessary data for creating a new inventory item
* 새로운 아이템을 생성하는데 필요한 데이터들이 ItemManifest에 존재.
*/
class UInv_InventoryItem;

USTRUCT(BlueprintType)
struct INVENTORY_API FInv_ItemManifest
{
	GENERATED_BODY()
public:
	UInv_InventoryItem* Manifest(UObject* NewOuter);

	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory = EInv_ItemCategory::None;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;
};