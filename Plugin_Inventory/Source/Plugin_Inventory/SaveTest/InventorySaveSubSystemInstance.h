// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventorySaveSubSystemInstance.generated.h"

/**
 * 
 */
class UInventorySave;
class USaveGame;

UCLASS()
class PLUGIN_INVENTORY_API UInventorySaveSubSystemInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void Save(APlayerController* PC);
	void LoadGame();
	void ApplyLoadData(APlayerController* PC);
private:
	UPROPERTY()
	TObjectPtr<UInventorySave> CachedSaveData;

	UPROPERTY()
	TSubclassOf<USaveGame> SaveClass;
};
