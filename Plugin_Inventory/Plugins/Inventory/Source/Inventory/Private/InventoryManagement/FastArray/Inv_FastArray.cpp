// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/FastArray/Inv_FastArray.h"

TArray<UInv_InventoryItem*> FInv_InventoryFastArray::GetAllItems() const
{
	return TArray<UInv_InventoryItem*>();
}

void FInv_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{

}
void FInv_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{

}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_ItemComponent* ItemComponent)
{
	return nullptr;
}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_InventoryItem* Item)
{
	return nullptr;
}

void FInv_InventoryFastArray::RemoveEntry(UInv_InventoryItem* Item)
{
}
