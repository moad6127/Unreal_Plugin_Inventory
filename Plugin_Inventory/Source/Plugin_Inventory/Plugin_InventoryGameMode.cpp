// Copyright Epic Games, Inc. All Rights Reserved.

#include "Plugin_InventoryGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "SaveTest/InventorySaveSubSystemInstance.h"



APlugin_InventoryGameMode::APlugin_InventoryGameMode()
{
	// stub
}

void APlugin_InventoryGameMode::BeginPlay()
{
	Super::BeginPlay();

	UInventorySaveSubSystemInstance* SaveSubSystem = GetGameInstance()->GetSubsystem<UInventorySaveSubSystemInstance>();
	SaveSubSystem->LoadGame();
}

void APlugin_InventoryGameMode::SaveSlotData()
{

}
