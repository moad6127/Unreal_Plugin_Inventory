// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveTest/InventorySaveSubSystemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "SaveTest/InventorySave.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"

void UInventorySaveSubSystemInstance::Save(APlayerController* PC)
{
	int32 SlotIndex = UInv_InventoryStatics::GetInventorySaveSlotIndex();
	FString SlotName = UInv_InventoryStatics::GetInventorySaveSlotName();

	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(SaveClass);
	CachedSaveData = Cast<UInventorySave>(SaveGameObject);
	UInv_InventoryComponent* InventoryComp = UInv_InventoryStatics::GetInventoryComponent(PC);
	CachedSaveData->InventoryData = InventoryComp->SaveInventoryItems();

	UGameplayStatics::SaveGameToSlot(CachedSaveData, SlotName, SlotIndex);
}

void UInventorySaveSubSystemInstance::LoadGame()
{
	int32 SlotIndex = UInv_InventoryStatics::GetInventorySaveSlotIndex();
	FString SlotName = UInv_InventoryStatics::GetInventorySaveSlotName();

	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		CachedSaveData = Cast<UInventorySave>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	else
	{
		if (SaveClass)
		{
			CachedSaveData = Cast<UInventorySave>(UGameplayStatics::CreateSaveGameObject(SaveClass));
		}
	}
}

void UInventorySaveSubSystemInstance::ApplyLoadData(APlayerController* PC)
{

}
