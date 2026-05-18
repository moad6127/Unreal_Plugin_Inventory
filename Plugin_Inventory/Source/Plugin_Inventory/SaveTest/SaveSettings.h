// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SaveSettings.generated.h"

/**
 * 
 */

class UInventorySave;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Save Settings"))
class PLUGIN_INVENTORY_API USaveSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Save")
	TSubclassOf<USaveGame> SaveClass;
};
