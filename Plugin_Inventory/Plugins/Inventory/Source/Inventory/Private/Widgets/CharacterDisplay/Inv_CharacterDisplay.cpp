// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterDisplay/Inv_CharacterDisplay.h"
#include "Kismet/GameplayStatics.h"
#include "EquipmentManagement/ProxyMesh/Inv_ProxyMesh.h"
#include "Blueprint/WidgetLayoutLibrary.h"



FReply UInv_CharacterDisplay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	LastPosition = CurrentPosition;

	bIsDragging = true;
	if (ProxyMesh.IsValid())
	{
		ProxyMesh->CaptureChange(bIsDragging);
	}
	return FReply::Handled();
}

FReply UInv_CharacterDisplay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsDragging = false;
	if (ProxyMesh.IsValid())
	{
		ProxyMesh->CaptureChange(bIsDragging);
	}
	return Super::NativeOnMouseButtonUp(InGeometry,InMouseEvent);
}

void UInv_CharacterDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bIsDragging = false;
	if (ProxyMesh.IsValid())
	{
		ProxyMesh->CaptureChange(bIsDragging);
	}
}

void UInv_CharacterDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AInv_ProxyMesh::StaticClass(), Actors);

	if (!Actors.IsValidIndex(0))
	{
		return;
	}

	ProxyMesh = Cast<AInv_ProxyMesh>(Actors[0]);
	if (!ProxyMesh.IsValid())
	{
		return;
	}

	Mesh = ProxyMesh->GetMesh();
}

void UInv_CharacterDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bIsDragging)
	{
		return;
	}

	LastPosition = CurrentPosition;
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	const float HorizontalDelta = LastPosition.X - CurrentPosition.X;
	if (!Mesh.IsValid())
	{
		return;
	}
	Mesh->AddRelativeRotation(FRotator(0.f, HorizontalDelta, 0.f));
}
