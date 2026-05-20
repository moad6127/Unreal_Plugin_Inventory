// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inv_InteractInterface.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None,
	Pickup,
	Save
};
USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EInteractionType Type = EInteractionType::None;

	UPROPERTY(BlueprintReadOnly)
	UObject* Payload = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInv_InteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INVENTORY_API IInv_InteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	bool Interact(FInteractionOption& OutOption);

	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	FString GetInfoMessage();
};
