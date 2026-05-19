


#include "Player/Inv_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Widgets/HUD/Inv_HUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Interaction/Inv_Highlightable.h"
#include "Interaction/Inv_InteractInterface.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"

AInv_PlayerController::AInv_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	TraceLength = 500.f;

	ItemTraceChannel = ECC_GameTraceChannel1;
}

void AInv_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceForItem();
}

void AInv_PlayerController::ToggleInventoryMenu()
{
	if (!InventoryComponent.IsValid())
	{
		return;
	}
	InventoryComponent->ToggleInventoryMenu();
	if (InventoryComponent->IsMenuOpen())
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AInv_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		for (UInputMappingContext* CurrentContext : DefaultIMCs)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}

	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();

	CreateHUDWidget();
}

void AInv_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &AInv_PlayerController::PrimaryInteract);
	EnhancedInputComponent->BindAction(ToggleInventoryMenuAction, ETriggerEvent::Started, this, &AInv_PlayerController::ToggleInventoryMenu);

}

void AInv_PlayerController::PrimaryInteract()
{
	if (!ThisActor.IsValid())
	{
		return;
	}
	FInteractionOption Option;
	Option.Payload = this;
	TArray<UActorComponent*> Components = ThisActor->GetComponentsByInterface(UInv_InteractInterface::StaticClass());
	if (!Components.IsEmpty())
	{
		for (UActorComponent* Component : Components)
		{
			if (IInv_InteractInterface::Execute_Interact(Component, Option))
			{
				break;
			}
		}
	}
	else
	{
		if (ThisActor->Implements<UInv_InteractInterface>())
		{
			IInv_InteractInterface::Execute_Interact(ThisActor.Get(), Option);
		}
	}
	HandleInteract(Option);

}

void AInv_PlayerController::HandleInteract(const FInteractionOption& Option)
{
	switch (Option.Type)
	{
	case EInteractionType::Pickup:
	{
		UInv_ItemComponent* ItemComp = Cast<UInv_ItemComponent>(Option.Payload);

		if (!IsValid(ItemComp) || !InventoryComponent.IsValid())
		{
			break;
		}

		InventoryComponent->TryAddItem(ItemComp);
		break;
	}
	case EInteractionType::Save:
	{

		break;
	}
	default:
		break;
	}
}

void AInv_PlayerController::CreateHUDWidget()
{
	if (!IsLocalController())
	{
		return;
	}

	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void AInv_PlayerController::TraceForItem()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return;
	}
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	const FVector2D ViewportCenter = ViewportSize / 2.f;

	FVector TraceStart;
	FVector Forward;

	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward))
	{
		return;
	}
	const FVector TraceEnd = TraceStart + Forward * TraceLength;

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel);

	LastActor = ThisActor;
	ThisActor = HitResult.GetActor();

	if (!ThisActor.IsValid())
	{
		if (IsValid(HUDWidget))
		{
			HUDWidget->HidePickupMessage();
		}
	}

	if (ThisActor == LastActor)
	{
		return;
	}
	if (ThisActor.IsValid())
	{
		UActorComponent* Highlightable = ThisActor->FindComponentByInterface(UInv_Highlightable::StaticClass());
		if (IsValid(Highlightable))
		{
			IInv_Highlightable::Execute_Highlight(Highlightable);
		}
		UInv_ItemComponent* ItemComp = ThisActor->FindComponentByClass<UInv_ItemComponent>();
		if (!IsValid(ItemComp))
		{
			return;
		}
		if (IsValid(HUDWidget))
		{
			HUDWidget->ShowPickupMessage(ItemComp->GetPickupMessage());
		}
	}

	if (LastActor.IsValid())
	{
		UActorComponent* Highlightable = LastActor->FindComponentByInterface(UInv_Highlightable::StaticClass());
		if (IsValid(Highlightable))
		{
			IInv_Highlightable::Execute_UnHighlight(Highlightable);
		}
	}
}


