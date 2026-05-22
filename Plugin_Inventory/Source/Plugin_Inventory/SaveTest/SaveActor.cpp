// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveTest/SaveActor.h"
#include "SaveTest/InventorySaveSubSystemInstance.h"

ASaveActor::ASaveActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PickupMessage = FString("E - Save Game");
}

bool ASaveActor::Interact_Implementation(FInteractionOption& OutOption)
{
	APlayerController* PC = Cast<APlayerController>(OutOption.Payload);
	UInventorySaveSubSystemInstance* SubSystem = GetGameInstance()->GetSubsystem<UInventorySaveSubSystemInstance>();
	SubSystem->Save(PC);
	OutOption.Type = EInteractionType::Save;
	OutOption.Payload = this;
	return true;
}

FString ASaveActor::GetInfoMessage_Implementation()
{
	return PickupMessage;
}

void ASaveActor::BeginPlay()
{
	Super::BeginPlay();
	
}


