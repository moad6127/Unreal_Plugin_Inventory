// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveTest/SaveActor.h"
#include "SaveTest/InventorySaveSubSystemInstance.h"

ASaveActor::ASaveActor()
{
	PrimaryActorTick.bCanEverTick = true;

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

void ASaveActor::BeginPlay()
{
	Super::BeginPlay();
	
}


