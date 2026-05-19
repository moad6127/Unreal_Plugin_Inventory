// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Inv_InteractInterface.h"
#include "SaveActor.generated.h"

UCLASS()
class PLUGIN_INVENTORY_API ASaveActor : public AActor, public IInv_InteractInterface
{
	GENERATED_BODY()
	
public:	
	ASaveActor();
	virtual bool Interact_Implementation(FInteractionOption& OutOption) override;

protected:
	virtual void BeginPlay() override;


};
