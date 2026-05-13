// Copyright Epic Games, Inc. All Rights Reserved.

#include "Plugin_InventoryGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Save/Inv_InventorySave.h"


APlugin_InventoryGameMode::APlugin_InventoryGameMode()
{
	// stub
}

void APlugin_InventoryGameMode::SaveSlotData()
{
	int32 SlotIndex = UInv_InventoryStatics::GetInventorySaveSlotIndex();
	FString SlotName = UInv_InventoryStatics::GetInventorySaveSlotName();

	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
	USaveGame* SaveGameObject =	UGameplayStatics::CreateSaveGameObject(SaveGameClass);
	UInv_InventorySave* InventorySaveGame = Cast<UInv_InventorySave>(SaveGameObject);

	UGameplayStatics::SaveGameToSlot(InventorySaveGame, SlotName, SlotIndex);
}
