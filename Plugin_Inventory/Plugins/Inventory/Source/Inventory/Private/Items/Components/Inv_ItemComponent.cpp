

#include "Items/Components/Inv_ItemComponent.h"
#include "Net/UnrealNetwork.h"

UInv_ItemComponent::UInv_ItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PickupMessage = FString("E - Pick UP");
	SetIsReplicatedByDefault(true);
}

void UInv_ItemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
}

bool UInv_ItemComponent::Interact_Implementation(FInteractionOption& OutOption)
{
	OutOption.Type = EInteractionType::Pickup;
	OutOption.Payload = this;
	return true;
}

FString UInv_ItemComponent::GetInfoMessage_Implementation()
{
	return PickupMessage;
}

void UInv_ItemComponent::InitItemManifest(FInv_ItemManifest CopyOfManifest)
{
	ItemManifest = CopyOfManifest;
}

void UInv_ItemComponent::PickedUp()
{
	OnPickedUP();
	GetOwner()->Destroy();
}

