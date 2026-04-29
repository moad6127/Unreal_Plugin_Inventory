// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"
#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"

#include "Items/Inv_InventoryItem.h"

 
void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equippables->OnClicked.AddDynamic(this, &UInv_SpatialInventory::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &UInv_SpatialInventory::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &UInv_SpatialInventory::ShowCraftables);

	Grid_Equippable->SetOwningCanvas(CanvasPanel);
	Grid_Equippable->EquipButtonClick.AddDynamic(this, &UInv_SpatialInventory::OnEquipButton);

	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);

	ShowEquippables();

	WidgetTree->ForEachWidget([this](UWidget* Widget) 
		{
			UInv_EquippedGridSlot* EquippedGridSlot = Cast<UInv_EquippedGridSlot>(Widget);
			if (IsValid(EquippedGridSlot))
			{
				EquippedGridSlots.Add(EquippedGridSlot);
				EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &UInv_SpatialInventory::EquippedGridSlotClicked);
			}
		});
}

FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ActiveGrid->HasHoverItem())
	{
		ActiveGrid->DropItem();
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInv_SpatialInventory::NativeTick(const FGeometry& InGeometry, float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);

	if (!IsValid(ItemDescription))
	{
		return;
	}
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
}

void UInv_SpatialInventory::SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description, UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS))
	{
		return;
	}
	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);
	
	FVector2D ClampedPosition = UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer())
	);

	ItemDescriptionCPS->SetPosition(ClampedPosition);
}

bool UInv_SpatialInventory::CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid())
	{
		return false;
	}
	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem))
	{
		return false;
	}
	UInv_InventoryItem* HeldItem = HoverItem->GetInventoryItem();
	bool bHasHoverItem = HasHoverItem();
	bool bHeldItme = IsValid(HeldItem);
	bool bNotStackable = !HoverItem->IsStackable();
	bool bEquipable = HeldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equippable;
	bool bEquipmentTypeTag = HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
	return bHasHoverItem && bHeldItme && bNotStackable && bEquipable && bEquipmentTypeTag;
}

UInv_EquippedGridSlot* UInv_SpatialInventory::FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([EquippedItem](UInv_EquippedGridSlot* GridSlot)
		{
			return GridSlot->GetInventoryItem() == EquippedItem;
		});
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

UInv_EquippedGridSlot* UInv_SpatialInventory::FindSlotWithEquipmentTypeTag(const FGameplayTag& EquipmentTypeTag) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([EquipmentTypeTag](UInv_EquippedGridSlot* GridSlot)
		{
			return EquipmentTypeTag.MatchesTag(GridSlot->GetEquipmentTypeTag());
		});
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

void UInv_SpatialInventory::ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

void UInv_SpatialInventory::RemoveEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	// ( OnEquippedSlottedItemClicked UnBind하기)
	// Removing the Equipped Slotted Item from Parent
	if (!IsValid(EquippedSlottedItem))
	{
		return;
	}
	if (EquippedSlottedItem->OnEquippeedSlottedItemClicked.IsAlreadyBound(this, &UInv_SpatialInventory::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippeedSlottedItemClicked.RemoveDynamic(this, &UInv_SpatialInventory::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UInv_SpatialInventory::MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem, UInv_EquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot))
	{
		return;
	}

	UInv_EquippedSlottedItem* SlottedItem = EquippedGridSlot->OnItemEquipped(
		ItemToEquip,
		EquippedSlottedItem->GetEquipmentTypeTag(),
		GetTileSize());
	if (IsValid(SlottedItem))
	{
		SlottedItem->OnEquippeedSlottedItemClicked.AddDynamic(this, &UInv_SpatialInventory::EquippedSlottedItemClicked);
	}
	EquippedGridSlot->SetEquippedSlottedItem(SlottedItem);
}

void UInv_SpatialInventory::BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const
{
	UInv_InventoryComponent* InventoryComp = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComp));
	InventoryComp->Server_EquipSlotClicked(ItemToEquip, ItemToUnequip);

}

FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent) const
{
	switch (UInv_InventoryStatics::GetItemCategoryFromItemComp(ItemComponent))
	{
	case EInv_ItemCategory::Equippable:
		return Grid_Equippable->HasRoomForItem(ItemComponent);
	case EInv_ItemCategory::Consumable:
		return Grid_Consumables->HasRoomForItem(ItemComponent);
	case EInv_ItemCategory::Craftable:
		return Grid_Craftables->HasRoomForItem(ItemComponent);
	default:
		UE_LOG(LogInventory, Error, TEXT("ItemComponent에 Category를 설정해야 합니다."))
		return FInv_SlotAvailabilityResult();
	}
}

void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UInv_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	DescriptionWidget->Collapse();

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);

	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this,&Manifest, DescriptionWidget]()
		{
			Manifest.AssimilateInventoryFragments(DescriptionWidget);
			GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
		});

	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer, DescriptionTimerDelegate, DescriptionTimerDelay, false);
}

void UInv_SpatialInventory::OnItemUnhovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}

UInv_ItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

bool UInv_SpatialInventory::HasHoverItem() const
{
	if (Grid_Equippable->HasHoverItem())
	{
		return true;
	}
	if (Grid_Consumables->HasHoverItem())
	{
		return true;
	}
	if (Grid_Craftables->HasHoverItem ())
	{
		return true;
	}
	return false;
}

UInv_HoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid())
	{
		return nullptr;
	}
	return ActiveGrid->GetHoverItem();
}

float UInv_SpatialInventory::GetTileSize() const
{
	return Grid_Equippable->GetTileSize();
}

void UInv_SpatialInventory::ShowEquippables()
{
	SetActiveGrid(Grid_Equippable, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UInv_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

void UInv_SpatialInventory::EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag)
{
	// hoveritem을 장착할수 있는지 체크
	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentTypeTag))
	{
		return;
	}
	UInv_HoverItem* HoverItem = GetHoverItem();
	// Equipped Slotted Item을 만들고 EquipGridSlot에 장착하기
	const float TileSize = UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();
	UInv_EquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		HoverItem->GetInventoryItem(),
		EquipmentTypeTag,
		TileSize
	);
	EquippedSlottedItem->OnEquippeedSlottedItemClicked.AddDynamic(this, &UInv_SpatialInventory::EquippedSlottedItemClicked);

	// 아이템을 Equip한것을 서버에 알리기(멀티플레이전용)(Unequip도 같은 것으로)
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquipSlotClicked(HoverItem->GetInventoryItem(), nullptr);

	// HoverItem을 Clear하기
	Grid_Equippable->ClearHoverItem();
}

void UInv_SpatialInventory::EquippedSlottedItemClicked(UInv_EquippedSlottedItem* SlottedItem)
{
	//장착된 아이템의 GridSlot을 Click했을때 일어나야 하는 상황들

	//  Item Description 제거
	UInv_InventoryStatics::ItemUnhovred(GetOwningPlayer());

	if (IsValid(GetHoverItem()) && GetHoverItem()->IsStackable())
	{
		return;
	}
	// 해당 타입의 EquippedGridSlot가져오기
		// 장착할 아이템 가져오기
		// Unequip할 장비 가져오기
	UInv_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;
	UInv_InventoryItem* ItemToUnequip = SlottedItem->GetInventoryItem();
	UInv_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	// 해당 타입의 EquippedGridSlot Clear하기 (set it's inventory item to nullptr)
	ClearSlotOfItem(EquippedGridSlot);

	// 이전에 장착된 아이템을 새로운 HoverItem으로 할당하기
	Grid_Equippable->AssignHoverItem(ItemToUnequip);

	// EquippedGridSlot 의 아이템을 제거하기
	RemoveEquippedSlottedItem(SlottedItem);
	// 새로운 EquippedSlottedItem만들기 (새로 장비할 아이템)
	MakeEquippedSlottedItem(SlottedItem, EquippedGridSlot, ItemToEquip);
	// OnItemEquipped/OnItemUnequipped Broadcast 하기  (from the IC)
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

void UInv_SpatialInventory::OnEquipButton(UInv_InventoryItem* Item,int32 Index)
{
	// EquippedGridSlot을 알기위해서 Item의 tag를 사용해서 GridSlot알아내기
	FGameplayTag ItemTag = Item->GetItemManifest().GetItemType();
	UInv_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquipmentTypeTag(ItemTag);
	if (!IsValid(EquippedGridSlot))
	{
		return;
	}
	FGameplayTag EquipmentTag = EquippedGridSlot->GetEquipmentTypeTag();
	UInv_InventoryItem* ItemToUnequip = EquippedGridSlot->GetInventoryItem().IsValid() ? EquippedGridSlot->GetInventoryItem().Get() : nullptr;
	// 장착할때 이미 장착된게 있으면 해당 아이템을 임시 저장하기
	if (ItemToUnequip)
	{
		//기존의 GridSlot에 저장된 아이템들을 제거하기
		ClearSlotOfItem(EquippedGridSlot);
		RemoveEquippedSlottedItem(EquippedGridSlot->GetEquippedSlottedItem());
	}
	// UInv_EquippedSlottedItem을 생성한후 장착하기
	UInv_EquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		Item,
		EquipmentTag,
		GetTileSize()
	);
	EquippedSlottedItem->OnEquippeedSlottedItemClicked.AddDynamic(this, &UInv_SpatialInventory::EquippedSlottedItemClicked);
	EquippedGridSlot->SetEquippedSlottedItem(EquippedSlottedItem);
	EquippedGridSlot->SetOccupiedTexture();
	//인벤토리의 아이템 제거하기
	Grid_Equippable->RemoveItemFromGrid(Item, Index);

	// 아이템을 Equip한것을 서버에 알리기(멀티플레이전용)(Unequip도 같은 것으로)
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquipSlotClicked(Item, ItemToUnequip);
	//임시 저장된 아이템을 다시 인벤토리에 넣기
	//TryAddItem을 하는게 좋은데 이건 ItemComp가 필요해서 Drop에서 사용한 방법으로 해야하나?
	if (ItemToUnequip)
	{
		Grid_Equippable->AddItemAtIndex(ItemToUnequip, Index, ItemToUnequip->IsStackable(), ItemToUnequip->GetTotalStackCount());
		Grid_Equippable->UpdateGridSlot(ItemToUnequip, Index, ItemToUnequip->IsStackable(), ItemToUnequip->GetTotalStackCount());
	}
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
		ActiveGrid->OnHide();
	}
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->ShowCursor();
	}
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}

void UInv_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);

	Button->SetIsEnabled(false);
}

