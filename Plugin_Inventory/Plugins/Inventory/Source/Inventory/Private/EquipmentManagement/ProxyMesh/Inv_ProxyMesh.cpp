// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/ProxyMesh/Inv_ProxyMesh.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "GameFramework/Character.h"
#include "Components/SceneCaptureComponent2D.h"

AInv_ProxyMesh::AInv_ProxyMesh()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	EquipmentComponent = CreateDefaultSubobject<UInv_EquipmentComponent>("EquipmentComp");
	EquipmentComponent->SetOwningSkeletalMesh(Mesh);
	EquipmentComponent->SetIsProxy(true);
	EquipmentComponent->OnEquipmentChanged.AddUObject(this, &AInv_ProxyMesh::OnEquipmentChanged);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
	CaptureComponent->SetupAttachment(RootComponent);
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;
	CaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;


	/*검은 바탕으로 만들기*/
	/*
	CaptureComponent->ShowFlags.SetAtmosphere(false);
	CaptureComponent->ShowFlags.SetFog(false);
	CaptureComponent->ShowFlags.SetCloud(false);
	CaptureComponent->ShowFlags.SetVolumetricFog(false);
	*/
}

void AInv_ProxyMesh::OnEquipmentChanged()
{
	UpdateShowOnlyActors();

	GetWorld()->GetTimerManager().SetTimer(
		EquipChangedTimer,
		this,
		&AInv_ProxyMesh::CaptureOnce,
		ChangeTime,
		false);

}

void AInv_ProxyMesh::CaptureOnce()
{
	if (CaptureComponent)
	{
		CaptureComponent->CaptureScene();
	}
}
void AInv_ProxyMesh::UpdateShowOnlyActors()
{
	CaptureComponent->ShowOnlyActors.Empty();

	// 자기 자신
	CaptureComponent->ShowOnlyActors.Add(this);

	// Attach된 Actor들 전부 가져오기
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		CaptureComponent->ShowOnlyActors.Add(Actor);
	}
}

void AInv_ProxyMesh::CaptureChange(bool IsDragging)
{
	CaptureComponent->bCaptureEveryFrame = IsDragging;
}

void AInv_ProxyMesh::BeginPlay()
{
	Super::BeginPlay();
	DelayedInitializeOwner();
	if (RenderTarget)
	{
		CaptureComponent->TextureTarget = RenderTarget;
	}
	CaptureComponent->ShowOnlyActors.Add(this);
	GetWorld()->GetTimerManager().SetTimer(
		EquipChangedTimer,
		this,
		&AInv_ProxyMesh::CaptureOnce,
		ChangeTime,
		false);
}

void AInv_ProxyMesh::DelayedInitializeOwner()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		DelayedInitializtion();
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		DelayedInitializtion();
		return;
	}

	ACharacter* Character = Cast<ACharacter>(PC->GetPawn());
	if (!IsValid(Character))
	{
		DelayedInitializtion();
		return;
	}
	USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	if (!IsValid(CharacterMesh))
	{
		DelayedInitializtion();
		return;
	}
	SourceMesh = CharacterMesh;
	Mesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
	Mesh->SetAnimInstanceClass(SourceMesh->GetAnimInstance()->GetClass());

	EquipmentComponent->InitializeOwner(PC);
}

void AInv_ProxyMesh::DelayedInitializtion()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AInv_ProxyMesh::DelayedInitializeOwner);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}






