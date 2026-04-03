// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Items/Inv_InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInv_HoverItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}
void UInv_HoverItem::UpdateStackCount(int32 Count)
{
	StackCount = Count;
	if (Count > 0)
	{
		Text_HoverStackCount->SetText(FText::AsNumber(Count));
		Text_HoverStackCount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Text_HoverStackCount->SetVisibility(ESlateVisibility::Collapsed);

	}
}

FGameplayTag UInv_HoverItem::GetItemType() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem->GetItemManifest().GetItemType();
	}
	return FGameplayTag();
}

void UInv_HoverItem::SetIsStackable(bool bStacks)
{
	bIsStackable = bStacks;
	if (!bStacks)
	{
		Text_HoverStackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}
UInv_InventoryItem* UInv_HoverItem::GetInventoryItem() const
{
	return InventoryItem.Get();
}

void UInv_HoverItem::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UInv_HoverItem::ClearItem()
{
	SetInventoryItem(nullptr);
	SetIsStackable(false);
	SetPreviousGridIndex(INDEX_NONE);
	UpdateStackCount(0);
	SetImageBrush(FSlateNoResource());

	RemoveFromParent();
}