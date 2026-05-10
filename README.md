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


</p>
</details>
<br/> <br>


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


## Inventory

## Item

## Equipment
