// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_ItemFragment.generated.h"

class APlayerController;

//Fragment를 사용해서 manifest에 표시할 내용들을 표시하기
//가장 최상위의 Fragment로 모든 Fragment는 해당 구조체를 부모로 두고 사용해야한다.
//따라서 가상 소멸자를 사용해서 혹시 모를 버그를 제거한다.
USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()
public:

	FInv_ItemFragment() {}
	FInv_ItemFragment(const FInv_ItemFragment&) = default;
	FInv_ItemFragment& operator=(const FInv_ItemFragment&) = default;
	FInv_ItemFragment(FInv_ItemFragment&&) = default;
	FInv_ItemFragment& operator=(FInv_ItemFragment&&) = default;

	virtual ~FInv_ItemFragment() {}

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }
	virtual void Manifest() {}
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "FragmentTags"))
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};

/*
* 위젯의 assimilation을 위한 Fragment
*/
class UInv_CompositeBase;

USTRUCT(BlueprintType)
struct FInv_InventoryItemFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UInv_CompositeBase* Composite) const;
protected:
	bool MatchesWidgetTag(const UInv_CompositeBase* Composite) const;
};

/*
* 인벤토리의 Grid에 사용할 정보들을 담기위한 Fragment이다.
*/
USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()
public:
	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint& Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{ 1,1 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding = 0.f;
};

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
};

USTRUCT(BlueprintType)
struct FInv_TextFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	FText GetText()const { return FragmentText; }
	void SetText(const FText& Text) { FragmentText = Text; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
};

USTRUCT(BlueprintType)
struct FInv_LabelNumberFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	float GetValue() const { return Value; }
	//처음 Manifest가 실행되면 랜덤값이 설정되도록 체크하는 bool변수
	bool bRandomizeOnManifest = true;
private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label{};

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value = 0.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min = 0.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max = 0.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel = false;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue = false;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits = 1;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits = 1;
};

USTRUCT(BlueprintType)
struct FInv_StackableFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount = 1;
};

// Consume Fragments
USTRUCT(BlueprintType)
struct FInv_ConsumeModifire : public FInv_LabelNumberFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) {}

};

USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
	virtual void OnConsume(APlayerController* PC);
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void Manifest() override;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ConsumeModifire>> ConsumeModifires;
};

USTRUCT(BlueprintType)
struct FInv_HealthPotionFragment : public FInv_ConsumeModifire
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;

private:

};

USTRUCT(BlueprintType)
struct FInv_ManaPotionFragment : public FInv_ConsumeModifire
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;

private:

};

/*
* Equipment
* 
*/
USTRUCT(BlueprintType)
struct FInv_EquipModifier : public FInv_LabelNumberFragment
{
	GENERATED_BODY()
public:
	virtual void OnEquip(APlayerController* PC) {}
	virtual void OnUnequip(APlayerController* PC) {}

private:

};

USTRUCT(BlueprintType)
struct FInv_StrengthModifier : public FInv_EquipModifier
{
	GENERATED_BODY()
public:
	virtual void OnEquip(APlayerController* PC) override;
	virtual void OnUnequip(APlayerController* PC) override;

private:

};


class AInv_EquipActor;

USTRUCT(BlueprintType)
struct FInv_EquipmentFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
public:
	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void Manifest() override;
	FGameplayTag GetEquipmentType() const { return EquipmentType; }

	AInv_EquipActor* SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const;
	void DestroyAttachedActor() const;
	void SetEquippedActor(AInv_EquipActor* EquipActor);

	bool bEquipped = false;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<TInstancedStruct<FInv_EquipModifier>> EquipModifiers;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AInv_EquipActor> EquipActorClass = nullptr;

	TWeakObjectPtr<AInv_EquipActor> EquippedActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName SocketAttackPoint = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EquipmentType = FGameplayTag::EmptyTag;
};