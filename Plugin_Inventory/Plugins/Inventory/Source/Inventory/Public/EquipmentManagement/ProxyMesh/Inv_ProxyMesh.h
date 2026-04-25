// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inv_ProxyMesh.generated.h"

class UInv_EquipmentComponent;

UCLASS()
class INVENTORY_API AInv_ProxyMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	AInv_ProxyMesh();
	USkeletalMeshComponent* GetMesh() const { return Mesh; }
protected:
	virtual void BeginPlay() override;

private:
	void DelayedInitializeOwner();
	void DelayedInitializtion();

	FTimerHandle TimerForNextTick;

	/*플레이어가 Controll하는 Mesh*/
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInv_EquipmentComponent> EquipmentComponent;

	/*인벤토리에서 보여줄 ProxyMesh*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh;
}; 
