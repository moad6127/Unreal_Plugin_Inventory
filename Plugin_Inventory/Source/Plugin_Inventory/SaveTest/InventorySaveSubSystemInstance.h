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
class UInv_InventoryComponent;
UCLASS()
class PLUGIN_INVENTORY_API UInventorySaveSubSystemInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//InventorySave를 테스트하기위해 임시 프로젝트에 만든 함수들
	void Save(APlayerController* PC);
	void LoadGame();
	void ApplyLoadData(APlayerController* PC);
	void RequestApply(ACharacter* Character);

	UFUNCTION()
	void HandleInventoryCompConstruct(UInv_InventoryComponent* InventoryComp);
private:
	UPROPERTY()
	TObjectPtr<UInventorySave> CachedSaveData;

	UPROPERTY()
	bool bLoadCompleted = false;

	UPROPERTY()
	TArray<TWeakObjectPtr<ACharacter>> WaitLoadCharacter;
};
