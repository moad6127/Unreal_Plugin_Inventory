// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveTest/InventorySaveSubSystemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "SaveTest/InventorySave.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "SaveTest/SaveSettings.h"

void UInventorySaveSubSystemInstance::Save(APlayerController* PC)
{
	int32 SlotIndex = UInv_InventoryStatics::GetInventorySaveSlotIndex();
	FString SlotName = UInv_InventoryStatics::GetInventorySaveSlotName();

	const USaveSettings* Settings = GetDefault<USaveSettings>();
	if (!Settings)
	{
		return;
	}
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}

	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(Settings->SaveClass);
	CachedSaveData = Cast<UInventorySave>(SaveGameObject);
	UInv_InventoryComponent* InventoryComp = UInv_InventoryStatics::GetInventoryComponent(PC);
	if (!IsValid(InventoryComp))
	{
		return;
	}
	CachedSaveData->InventoryData = InventoryComp->SaveInventoryItems();

	UGameplayStatics::SaveGameToSlot(CachedSaveData, SlotName, SlotIndex);
	UE_LOG(LogTemp, Warning, TEXT("Save!"));

}

void UInventorySaveSubSystemInstance::LoadGame()
{
	int32 SlotIndex = UInv_InventoryStatics::GetInventorySaveSlotIndex();
	FString SlotName = UInv_InventoryStatics::GetInventorySaveSlotName();
	
	const USaveSettings* Settings = GetDefault<USaveSettings>();
	if (!Settings)
	{
		return;
	}
	
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		CachedSaveData = Cast<UInventorySave>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	else
	{
		if (Settings->SaveClass)
		{
			CachedSaveData = Cast<UInventorySave>(UGameplayStatics::CreateSaveGameObject(Settings->SaveClass));
			UGameplayStatics::SaveGameToSlot(CachedSaveData, SlotName, SlotIndex);
		}
	}
}

void UInventorySaveSubSystemInstance::ApplyLoadData(APlayerController* PC)
{
	UInv_InventoryComponent* InventoryComp = UInv_InventoryStatics::GetInventoryComponent(PC);

	UE_LOG(LogTemp, Warning, TEXT("InventoryComp : %s"),*InventoryComp->GetName());
}
