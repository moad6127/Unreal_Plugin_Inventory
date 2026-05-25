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





### ItemDescription

#### Composite 패턴

------------------------------------------------------
## Item

### Item Component
### ItemManifest
### ItemFragment

---------------------------------------
## Equipment

### EquipmentComponent
### EquipActor
### ProxyMesh
### CharacterDisplay


--------------------------------------------
## InventorySave
