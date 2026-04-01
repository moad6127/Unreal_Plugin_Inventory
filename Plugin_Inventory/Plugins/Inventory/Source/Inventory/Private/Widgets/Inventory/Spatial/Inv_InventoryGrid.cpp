// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Components/CanvasPanel.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Inventory.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ConstructGrid();
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &UInv_InventoryGrid::AddStacks);


}

void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const FVector2D CanvasPosition = UInv_WidgetUtils::GetWidgetPosision(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	UpdateTileParameters(CanvasPosition, MousePosition);
}

void UInv_InventoryGrid::UpdateTileParameters(const FVector2D CanvasPosition, const FVector2D MousePosition)
{
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	LastTIleParameters = TileParameters;
	TileParameters.TileCoordinates = HoveredTileCoordinates;
	TileParameters.TileIndex = UInv_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);

	OnTileParametersUpdate(TileParameters);
}

void UInv_InventoryGrid::OnTileParametersUpdate(const FInv_TileParameters& Parameters)
{
	if (!IsValid(HoverItem))
	{
		return;
	}

	//Hovered Item의 Dimension을 가져오기.
	const FIntPoint Dimensions = HoverItem->GetGridDemensions();
	//타일 Highlight를 위해 시작 좌표 계산하기.
	const FIntPoint StartingCoordinate = CalculateStartingCoordinates(Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);

}

FIntPoint UInv_InventoryGrid::CalculateStartingCoordinates(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EInv_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoord;
	switch (Quadrant)
	{
	case EInv_TileQuadrant::TopLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EInv_TileQuadrant::TopRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EInv_TileQuadrant::BottomLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	case EInv_TileQuadrant::BottonRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;

	default:
		UE_LOG(LogInventory, Error, TEXT("Invalid Quadrant"));
		return FIntPoint(-1, -1);
	}
	return StartingCoord;
}

FIntPoint UInv_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D MousePosition) const
{
	return FIntPoint{
		FMath::FloorToInt32((MousePosition.X - CanvasPosition.X) / TileSize),
		FMath::FloorToInt32((MousePosition.Y - CanvasPosition.Y) / TileSize)
	};
}

EInv_TileQuadrant UInv_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D MousePosition) const
{
	//현재 타일 내 상대 위치 계산
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, TileSize);

	//Determine which quadrant the mouse is in

	const bool bIsTop = TileLocalY < TileSize / 2.f; 
	const bool bIsLeft = TileLocalX < TileSize / 2.f;

	EInv_TileQuadrant HoveredTileQuadrant = EInv_TileQuadrant::None;
	if (bIsTop && bIsLeft)
	{
		HoveredTileQuadrant = EInv_TileQuadrant::TopLeft;
	}
	else if (bIsTop && !bIsLeft)
	{
		HoveredTileQuadrant = EInv_TileQuadrant::TopRight;
	}
	else if (!bIsTop && bIsLeft)
	{
		HoveredTileQuadrant = EInv_TileQuadrant::BottomLeft;
	}
	else if (!bIsTop && !bIsLeft)
	{
		HoveredTileQuadrant = EInv_TileQuadrant::BottonRight;
	}

	return HoveredTileQuadrant;
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemCompoennt)
{
	return HasRoomForItem(ItemCompoennt->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* Item)
{
	return HasRoomForItem(Item->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& Manifest)
{
	FInv_SlotAvailabilityResult Result;

	// Determine if the item is stackable
	//항목을 쌓을 수 있는지 확인
	const FInv_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	//Determine how may stacks to add.
	//얼마나 stack을 할지 확인
	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;

	TSet<int32> CheckedIndices;
	//For each Grid Slot
	//각각의 GridSlot이 다음 조건을 만족하는지 확인
	for (const auto& GridSlot : GridSlots)
	{
		//if we don't have anymore to fill, break out of the loop early.
		// 채울필요가 없다면 반복문 탈출
		if (AmountToFill == 0)
		{
			break;
		}
		//if this index claimed yet?
		// index가 사용되고 있는지 확인
		if (IsIndexClaimed(CheckedIndices,GridSlot->GetTileIndex()))
		{
			continue;
		}
		//Is the item in grid bounds?
		// 범위를 나가는지 확인
		if (!IsInGridBounds(GridSlot->GetTileIndex(), GetItemDimensions(Manifest)))
		{
			continue;
		}

		//can the item fit here?
		// 아이템 들어갈수 있는지 확인
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot,GetItemDimensions(Manifest),CheckedIndices,TentativelyClaimed,Manifest.GetItemType(),MaxStackSize))
		{
			continue;
		}
		// how much to fill?
		// 얼마나 채우나?

		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if (AmountToFillInSlot == 0)
		{
			continue;
		}

		CheckedIndices.Append(TentativelyClaimed);

		// update the amount left to fill
		// 왼쪽부터 최신화 하기
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FInv_SlotAvailability{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetTileIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);

		AmountToFill -= AmountToFillInSlot;

		//how much is the Remainder
		//나머지는 어느정도인지 확인
		Result.Remainder = AmountToFill;
		if (AmountToFill == 0)
		{
			return Result;
		}
	}

	return Result;
}

bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckIndices, const int32 Index) const
{
	return CheckIndices.Contains(Index);
}

bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize)
{
	//is ther room at this index? 
	// 다른 아이템이 있는지 확인

	bool bHasRoomAtIndex = true;

	UInv_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetTileIndex(), Dimensions, Columns,
		[&](const UInv_GridSlot* SubGridSlot)
		{
			if (CheckSlotConstraints(GridSlot,SubGridSlot,CheckedIndices,ItemType,MaxStackSize))
			{
				OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
			}
			else
			{
				bHasRoomAtIndex = false;
			}
		});

	return bHasRoomAtIndex;
}

bool UInv_InventoryGrid::CheckSlotConstraints(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot, const TSet<int32>& CheckedIndices, const FGameplayTag& ItemType, const int32 MaxStackSize) const
{
	// item claimdd?
	// index가 사용중인가?
	if (IsIndexClaimed(CheckedIndices,SubGridSlot->GetTileIndex()))
	{
		return false;
	}

	// has vaild item?
	// 	item 이 존재하는가?
	if (!HasValidItem(SubGridSlot))
	{

		return true;
	}

	//Is this Grid slot an upper left slot?
	//해당 GridSlot이 UpperLeft인가?
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot))
	{
		return false;
	}
	//  is this a stackable item?
	// 그아이템은 stackable아이템인가?
	UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable())
	{
		return false;
	}

	// is this item same type as the item we're trying to add?
	// 같은타입의 아이템이 있는데 추가하려고 하는가?
	if (!DoesItemTypeMatch(SubItem, ItemType))
	{
		return false;
	}
	// if stackable, is this slot at the max stack size already?
	// stackable아이템을때 이미 stack전부가 채워져있나?
	if (GridSlot->GetStackCount() >= MaxStackSize)
	{
		return false;
	}

	return true;
}

bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetTileIndex();
}

bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInv_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const 
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num())
	{
		return false;
	}
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

FIntPoint UInv_InventoryGrid::GetItemDimensions(const FInv_ItemManifest& Manifest) const
{
	const FInv_GridFragment* GridFragment = Manifest.GetFragmentOfType<FInv_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);

}

int32 UInv_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot) const
{
	//슬롯에 들어갈수 있는 양 계산
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);

	//AmountToFill과 슬롯에 들어갈수 있는 양 사이에서 최소값을 넣기
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
}

bool UInv_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInv_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	//HoverItem연결, Grid에 있는 Clicked아이템제거
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UInv_InventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment)
	{
		return;
	}
	UInv_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns,
		[&](UInv_GridSlot* GridSlot) 
		{
			GridSlot->SetInventoryItem(nullptr);
			GridSlot->SetUpperLeftIndex(INDEX_NONE);
			GridSlot->SetUnoccupiedTexture();
			GridSlot->SetAvailable(true);
			GridSlot->SetStackCount(0);
		});
	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInv_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(InventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment)
	{
		return;
	}

	const FVector2D DrawSize = GetDrawSize(GridFragment);

	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}


void UInv_InventoryGrid::AddStacks(const FInv_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get()))
	{
		return;
	}
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlot(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UInv_InventoryGrid::OnSlottedItemClied(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlots.IsValidIndex(GridIndex));

	UInv_InventoryItem* CliedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(CliedInventoryItem, GridIndex);
	}
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	if (!MatchesCategory(Item))
	{
		return;
	}
	FInv_SlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlot(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}

}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment)
	{
		return;
	}

	UInv_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);

	SlottedItems.Add(Index, SlottedItem);
}

UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment, const int32 Index)
{
	UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &UInv_InventoryGrid::OnSlottedItemClied);

	return SlottedItem;
}

void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment, UInv_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));

	const FVector2D DrawPos = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns)* TileSize;

	const FVector2D DrawPosWithPaddin = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPosWithPaddin);
}

void UInv_InventoryGrid::UpdateGridSlot(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}


	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
	if (!GridFragment)
	{
		return;
	}
	const FIntPoint Dimensions = GridFragment->GetGridSize();
	
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(NewItem);
			GridSlot->SetUpperLeftIndex(Index);
			GridSlot->SetOccupiedTexture();
			GridSlot->SetAvailable(false);
		});
}



void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment)
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}


FVector2D UInv_InventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWith = TileSize - GridFragment->GetGridPadding() * 2;
	return GridFragment->GetGridSize() * IconTileWith;
}

void UInv_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int32 j = 0; j < Rows; j++)
	{
		for (int32 i = 0; i < Columns; i++)
		{
			UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);

			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));
			
			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);

			GridSlots.Add(GridSlot);
		}
	}
}



bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}

