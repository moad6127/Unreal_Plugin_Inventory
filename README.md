
# Unreal_Plugin_Inventory



공간형 인벤토리를 만들고 플러그인 형태로 만들어서 다른 프로젝트에서도 사용가능하도록 제작되었다.    
캐릭터의 Controller에 Inventory Component를 부착해서 사용할수 있으며,      
아이템을 필요한 조각들을 합성해서 만들수 있도록 구성해, 필요한 정보들을 넣을수 있다.     
아이템 구성또한 Component형태를 사용해서 다른프로젝트의 Actor에 부착해서 인벤토리의 기능을 사용할수 있도록 만들었다.    



<img width="1938" height="1058" alt="Image" src="https://github.com/user-attachments/assets/4591f4de-0f84-44f2-b258-ddfcaf81ba81" />

<details><summary> 구분</summary>
<p>  
  
 * [InventoryComponent](#InventoryComponent)
   
 * [Inventory](#Inventory)

 * [Item](#Item)

 * [Equipment](#Equipment)

 * [InventorySave](#InventorySave)


</p>
</details>
<br/> <br>

-----------------------------------------------------
## InventoryComponent

Player의 Controller에 Component로 부착되어 사용되는 InventoryComponent로 Inventory기능들을 담당하고 있다.      

- [InventoryComponent h](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/InventoryManagement/Components/Inv_InventoryComponent.h)      
- [InventoryComponent C++](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/InventoryManagement/Components/Inv_InventoryComponent.cpp)


``` C++
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void TryAddItem(UInv_ItemComponent* ItemComponent);

	UFUNCTION(Server,Reliable)
	void Server_AddNewItem(UInv_ItemComponent* ItemComponent, int32 StackCount , int32 Remainder);
	
	UFUNCTION(Server, Reliable)
	void Server_AddStacksToItem(UInv_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable)
	void Server_DropItem(UInv_InventoryItem* Item,int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_ConsumeItem(UInv_InventoryItem* Item);

    UPROPERTY(Replicated)
	FInv_InventoryFastArray InventoryList;
```
> 아이템의 추가, 버리기, 소비하기 등의 기능을 가지고 있으며 아이템을 저장할수 있는 변수도 가지고 있다.    

### FastArray

``` C++
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:

	FInv_InventoryEntry() {}

private:
	friend struct FInv_InventoryFastArray;
	friend UInv_InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;
};

/* List of inventory items*/
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()
public:
    .....

private:

	friend UInv_InventoryComponent;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	/*Replicated list of items*/
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;
};
```
> 아이템들 저장하는 변수들을 멀티 플레이이 환경에서 고성능을 내기위해 FastArray형태를 만들어서 사용중이다.

- [FastArray h](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/InventoryManagement/FastArray/Inv_FastArray.h)      
- [FastArray C++](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/InventoryManagement/FastArray/Inv_FastArray.cpp)

---------------------------------------------------------
## Inventory

<img width="295" height="402" alt="Image" src="https://github.com/user-attachments/assets/c477dc3d-ebc9-46e4-8a50-e09c7b07adb3" />

저장된 아이템을 UI로 표시할수 있도록 Widget으로 구성된 인벤토리들이 있다.      
따로 컨트롤러를 두어서 만들지 않고 각각의 Widget들의 클래스에서 함수를 만들어서 동작하도록 만들었다.       

### SpatialInventory

인벤토리의 전체적인 틀을 가지고 있고, 여러개의 Grid를 WidgetSwitcher 로 변경할수 있다.       

- [SpatialInventory h](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/Widgets/Inventory/Spatial/Inv_SpatialInventory.h)      
- [SpatialInventory C++](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/Widgets/Inventory/Spatial/Inv_SpatialInventory.cpp)


### InventoryGrid

<img width="264" height="168" alt="Image" src="https://github.com/user-attachments/assets/0afe23f7-1186-4cc3-9a7c-c2435df17386" />

- [InventoryGrid h](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/Widgets/Inventory/Spatial/Inv_InventoryGrid.h)      
- [InventoryGrid C++](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/Widgets/Inventory/Spatial/Inv_InventoryGrid.cpp)


인벤토리에 들어온 아이템들을 표시하는 공간으로 설정된 크기의 형태로 GirdSlot들을 부착시켜서 인벤토리의 형태를 구성했다.        
크게 3가지 분류인 Equipable,Consumables,Craftables로 나눠서 아이템의 종류에 따라서 저장하는 위치가 다르다.  

```
construct
```

```C++
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
			GridSlot->GridSlotClicked.AddDynamic(this, &UInv_InventoryGrid::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &UInv_InventoryGrid::OnGridSlotHovered);
			GridSlot->GridSlotUnHovered.AddDynamic(this, &UInv_InventoryGrid::OnGridSlotUnhovered);

		}
	}
}
```
> 처음 게임을 시작하여 인벤토리의 내부 공간을 구성할때 호출되는 함수이다.      
> GridSlot들을 에디터에서 설정된 Row와 Column값으로 붙여 내부 인벤토리를 구성하게 된다.      

```
AddItem
```
```C++
	// Player가 아이템을 넣을때 InventoryComp에서 아이템의 정보추가해서 이벤트를 보내게 된다.
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
...

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	if (!MatchesCategory(Item))
	{
		return;
	}
	FInv_SlotAvailabilityResult Result = HasRoomForItem(Item); // 아이템이 들어올 자리가 있는지 확인
	AddItemToIndices(Result, Item);
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill); // 아이템 UI를 만들고 추가
		UpdateGridSlot(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill); // GridSlot이 점유중인것으로 상태 변경
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

	UInv_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index); // Fragment를 사용해서 아이템UI를 생성후 정해진 위치에 넣기
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);

	SlottedItems.Add(Index, SlottedItem);
}
```

> 플레이어가 아이템을 획득했을때 InventoryComp에서 넘어온 Data를 바탕으로 UI를 변경해서 아이템을 화면에 표시하도록 만들었다.       
> 인벤토리에 아이템이 들어올 공간이 있는지 확인한후 아이템을 추가하도록 만들었다.      
> 에디터에서 설정된 아이템의 크기, 이미지등을 바탕으로 Slotted아이템을 만든후 넣을수 있는 공간에 아이템을 추가한다.        




### ItemPopUp

<img width="238" height="265" alt="ItemPopup" src="https://github.com/user-attachments/assets/15fc72f2-0a03-48cb-88f1-f56ecb8ddf84" />

아이템을 클릭했을때 나오도록 만든 ItemPopup메뉴로 각각 Split, Drop, Consume, Equip창이 존재하며 아이템의 Type에 따라서 보이는 창을 다르게 만들었다.        
```
Item Popup Create
```

```C++
void UInv_InventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	...

	const int32 SliderMax = GridSlots[GridIndex]->GetStackCount() - 1;
	if (RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this, &UInv_InventoryGrid::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax, FMath::Max(1, GridSlots[GridIndex]->GetStackCount() / 2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}

	ItemPopUp->OnDrop.BindDynamic(this, &UInv_InventoryGrid::OnPopUpMenuDrop);
	if (RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this, &UInv_InventoryGrid::OnPopUpMenuConsume);
	}
	else
	{
		ItemPopUp->CollapseConsumeButton();
	}
	if (RightClickedItem->IsEquippable())
	{
		ItemPopUp->OnEquip.BindDynamic(this, &UInv_InventoryGrid::OnPopUpMenuEquip);
	}
	else
	{
		ItemPopUp->CollapseEquipButton();
	}
}
```
> 아이템을 클릭하면 해당 함수가 호출이 되고 아이템의 Type에 따라서 Popup에서 보이는 버튼들을 다르게 만들었다.       
> 각각의 버튼은 해당 기능들과 연동되어 클릭하게 되면 해당 기능이 실행된다.    
> 아이템의 타입에 따라서 해당 기능들은 줄이도록 만들어 사용된다.      

```
SplitButton
```

SplitButton이 클릭되면 PopUp에서 설정한 값을 사용해서 분리하게 된다.
```C++
void UInv_InventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem))
	{
		return;
	}
	if (!RightClickedItem->IsStackable())
	{
		return;
	}
	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
	
}
```
> 클릭된 아이템과 분리할 개수를 정해서 오게되면 기존의 아이템의 StackCount를 줄이고 새로운 아이템을 HoverItem형태로 분리해서 움직일수 있게 만들었다.       

```
DropButton
```
Drop버튼을 누르게 되면 해당 아이템은 Drop하게 된다.

```C++
void UInv_InventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem))
	{
		return;
	}

	PickUp(RightClickedItem, Index);
	DropItem();
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	//HoverItem연결, Grid에 있는 Clicked아이템제거
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	// Drop하기위해서 Drop할 아이템을 먼저 Pickup한후 Pickup된 아이템을 Drop하도록 만들었다.
}
void UInv_InventoryGrid::DropItem()
{
	...

	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());
	// Drop은 InventoryComp에서 할수 있도록 만들었다.

	ClearHoverItem();
	ShowCursor();
}

```
> Drop버튼을 누르게 되면 먼저 Drop할 아이템을 Pickup하도록 만들어 HoverItem형태로 변경한뒤 해당 아이템을 Drop하도록 만들었다.

```
Consume Button
```
Consume 버튼을 누르게되면 해당 아이템을 소비하게 되며 설정되어있는 동작이 작동하게 된다.

```C++
void UInv_InventoryGrid::OnPopUpMenuConsume(int32 Index)
{
	...
	// 기존 Stack에서 하나를 줄이도록 만든다.
	const int32 NewStackCount = UpperLeftGridSlot->GetStackCount() - 1;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	InventoryComponent->Server_ConsumeItem(RightClickItem);

	// Count가 0이하이면 아이템을 제거한다.
	if (NewStackCount <= 0)
	{
		RemoveItemFromGrid(RightClickItem, Index);
	}
}

void UInv_InventoryComponent::Server_ConsumeItem_Implementation(UInv_InventoryItem* Item)
{

	...
	if (FInv_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningController.Get());
	}
}

// Fragment에서 Consume함수가 호출되며 모든ConsumeModifire 에서 함수가 작동하게된다.
void FInv_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (TInstancedStruct<FInv_ConsumeModifire>& Modifire : ConsumeModifires)
	{
		auto& ModRef = Modifire.GetMutable();
		ModRef.OnConsume(PC);
	}
}
void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	//PC를 사용해 게임구조에 맞는 Consume사용하기
	//예를 들어 GAS에서 사용할경우 PC에서 AbilitySystemComp를 가져와 GameplayEffect를 적용시키는 방법등이 있다.

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("HealthPotion consumed Healing by :%f"), GetValue()));
}
```
> Consume이 작동하게 되면 아이템의 개수를 한개를 줄이도록 만든후 해당 아이템이 0이하이면 제거하게 된다.     
> InventoryComponent에서 Server함수인 Consume이 동작하게 되고 해당 아이템의 Fragmemt에서 필요한 동작을 하게 된다.    

```
EquipButton
```

Equip버튼을 누르게 되면 해당 아이템을 장착할수 있게 된다.      

```C++
void UInv_InventoryGrid::OnPopUpMenuEquip(int32 Index)
{
	UInv_InventoryItem* RightClickItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickItem))
	{
		return;
	}

	EquipButtonClick.Broadcast(RightClickItem,Index);
}
void UInv_SpatialInventory::OnEquipButton(UInv_InventoryItem* Item,int32 Index)
{

	...
	FGameplayTag EquipmentTag = EquippedGridSlot->GetEquipmentTypeTag();
	UInv_InventoryItem* ItemToUnequip = EquippedGridSlot->GetInventoryItem().IsValid() ? EquippedGridSlot->GetInventoryItem().Get() : nullptr;
	// 장착할때 이미 장착된게 있으면 해당 아이템을 임시 저장하기
	if (ItemToUnequip)
	{
		//기존의 GridSlot에 저장된 아이템들을 제거하기
		RemoveEquippedSlottedItem(EquippedGridSlot->GetEquippedSlottedItem());
		ClearSlotOfItem(EquippedGridSlot);
	}

	...

	// 아이템을 Equip한것을 서버에 알리기(멀티플레이전용)(Unequip도 같은 것으로)
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquipSlotClicked(Item, ItemToUnequip);
	...
}
```
> Equip버튼을 누르게 되면 아이템을 장착하게 된다.



### ItemDescription

<img width="349" height="257" alt="Image" src="https://github.com/user-attachments/assets/3f2e3394-b433-4771-bbc6-17b6e5757064" />
<img width="349" height="257" alt="Image" src="https://github.com/user-attachments/assets/f1e48ac2-f6da-4dfb-aac5-2dcb35a1c2ef" />

아이템에 커서를 올려두고 있으면 나타나는 아이템 설명창으로 Composite디자인 패턴과 Item의 Fragment와 결합되어 저장된 데이터가 없을경우 UI로 표시하지 않게 만들었다.

```C++
void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UInv_ItemDescription* DescriptionWidget = GetItemDescription();
	CollapseDescription(DescriptionWidget);
	CollapseDescription(GetEquippedItemDescription());

	...
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this,Item,&Manifest, DescriptionWidget]()
		{
			Manifest.AssimilateInventoryFragments(DescriptionWidget);
			GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);

			...
		});
		...

}

```
> 인벤토리 내부의 아이템에 마우스 커서를 가져다가 두면 일정시간이 지난후에 아이템 설명창이 나오도록 만들었다.
> 각각의 아이템들은 전부 Type과 내부의 기능들이 다르기 때문에 Composite패턴을 사용해서 설정된 값들이 나오도록 만들었다.

#### Description Composite 패턴

CompositeBase를 최상위로 두고 Commposite와 Leaf를 사용해서 클라이언트는 동일한 인터페이스를 사용해서 모든 기능을 사용할수 있도록 만들어진 패턴이다.

```C++

void FInv_ItemManifest::AssimilateInventoryFragments(UInv_CompositeBase* Composite) const
{
	const auto& AllInventoryItemFragments = GetAllFragmentOfType<FInv_InventoryItemFragment>();
	for (const auto* Fragment : AllInventoryItemFragments)
	{
		Composite->ApplyFunction([Fragment](UInv_CompositeBase* Widget)
			{
				Fragment->Assimilate(Widget);
			});
	}
}

void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite))
	{
		return;
	}
	Composite->Expand();
}

void FInv_ImageFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	...
	// DOWORK
}

void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	...
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const TInstancedStruct<FInv_ConsumeModifire>& Modifire : ConsumeModifires)
	{
		const auto& ModRef = Modifire.Get();
		ModRef.Assimilate(Composite);
	}
}
```

> 이처럼 Description을 보여줄때 InventoryComponent의 AssimilateInventoryFragments함수를 호출해서 패턴기능을 통해 아이템에 설정된 모든 설명들을 표시하도록 만들었다.

------------------------------------------------------
## Item

- [Items h](https://github.com/moad6127/Unreal_Plugin_Inventory/tree/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/Items)      
- [Items C++](https://github.com/moad6127/Unreal_Plugin_Inventory/tree/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/Items)

인벤토리에 저장될 아이템들의 정보를 담고 있는 것들이다.     
 프로젝트에서 아이템으로 사용될 Actor클래스에 ItemComponent를 추가하면 간단하게 인벤토리에 넣을수 있다.    

### Item Component

- [Item Component h](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/Items/Components/Inv_ItemComponent.h)      
- [Item Component C++](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/Items/Components/Inv_ItemComponent.cpp)

프로젝트에서 아이템으로 사용되는 Actor에 추가해서 인벤토리 플러그인의 기능들을 사용할수 있게 만들어주는 Component이다.      

<img width="463" height="454" alt="Image" src="https://github.com/user-attachments/assets/e5ab807b-e773-4e10-87df-a929914ea17c" />

> 에디터에서 해당 컴포넌트의 기능들을 설저해서 인벤토리에 필요한 Data들을 저장하고 사용할수 있게 만들어져 있다.          
> 아이템들에는 단순히 Data만을 담고 있고 수행하는 기능들은 대부분 Widget이나 InventoryComponent에서 수행하고 있다.


### ItemManifest

- [ItemManifest h](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/Items/Manifest/Inv_ItemManifest.h)      
- [ItemManifest C++](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/Items/Manifest/Inv_ItemManifest.cpp)

아이템을 생성하는데 필요한 Data들을 가지고 있는 구조체로 Fragment, ItemType, ItemCategory등을 가지고 인벤토리에 아이템이 들어갈때 가지고 있어야할 Data들을 저장하고 있다.        


### ItemFragment

- [ItemFragment h](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Public/Items/Fragments/Inv_ItemFragment.h)      
- [ItemFragment C++](https://github.com/moad6127/Unreal_Plugin_Inventory/blob/master/Plugin_Inventory/Plugins/Inventory/Source/Inventory/Private/Items/Fragments/Inv_ItemFragment.cpp)

인벤토리와 UI에서 사용할 정보들을 담고 있는 구조체들로 최상위 계층과 그아래 계층들로 이루어져 있다.


```
Sample<Image Fragment>
```

```C++

/*
* 인벤토리에 표시할 Image정보를 담기위한 Fragment이다.
*/
USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	UTexture2D* GetIcon()const { return Icon; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{ 44.f,44.f };


	// 인벤토리에서 아이템의 Image를 설정할때 에디터에서 설정된 ImageFragment를 사용해서 사용하게 된다.
...
void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment)
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

};
```
<img width="430" height="170" alt="Image" src="https://github.com/user-attachments/assets/6b6329f4-0fff-4da5-828f-9560b84b624e" />

<img width="284" height="170" alt="Image" src="https://github.com/user-attachments/assets/72e387a6-4685-4939-baee-4190d9387a5e" />

> 인벤토리에서 사용될 Image를 설정하기위한 Fragment로 프로젝트에서 사용할 Actor에 부착할 ItemComponent에서 해당 Fragment를 설정해서 인벤토리의 아이템 이미지를 설정할수 있다.

 이처럼 다양한 Fragment를 사용해서 인벤토리 아이템의 기본 설정값등을 정해 인벤토리 시스템에서 사용할수 있도록 만들었다.




---------------------------------------
## Equipment

### EquipmentComponent
### EquipActor
### ProxyMesh
### CharacterDisplay


--------------------------------------------
## InventorySave
