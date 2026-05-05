// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Image.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"


void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	//PC를 사용해 게임구조에 맞는 Consume사용하기
	//예를 들어 GAS에서 사용할경우 PC에서 AbilitySystemComp를 가져와 GameplayEffect를 적용시키는 방법등이 있다.

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("HealthPotion consumed Healing by :%f"), GetValue()));
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("ManaPotion consumed Healing by :%f"), GetValue()));

}


void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite))
	{
		return;
	}
	Composite->Expand();
}

bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

void FInv_ImageFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite))
	{
		return;
	}
	UInv_Leaf_Image* Image = Cast<UInv_Leaf_Image>(Composite);
	if (!IsValid(Image))
	{
		return;
	}
	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}

void FInv_TextFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite))
	{
		return;
	}
	UInv_Leaf_Text* LeafText = Cast<UInv_Leaf_Text>(Composite);
	if (!IsValid(LeafText))
	{
		return;
	}
	LeafText->SetText(FragmentText);
}

void FInv_LabelNumberFragment::Manifest()
{
	FInv_ItemFragment::Manifest();

	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
	}
	bRandomizeOnManifest = false;
}

void FInv_LabelNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);

	if (!MatchesWidgetTag(Composite))
	{
		return;
	}
	UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue))
	{
		return;
	}
	LabeledValue->SetText_Label(Text_Label, bCollapseLabel);

	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;
	LabeledValue->SetText_Value(FText::AsNumber(Value, &Options), bCollapseValue);
}

void FInv_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (TInstancedStruct<FInv_ConsumeModifire>& Modifire : ConsumeModifires)
	{
		auto& ModRef = Modifire.GetMutable();
		ModRef.OnConsume(PC);

	}
}


void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const TInstancedStruct<FInv_ConsumeModifire>& Modifire : ConsumeModifires)
	{
		const auto& ModRef = Modifire.Get();
		ModRef.Assimilate(Composite);
	}
}

void FInv_ConsumableFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	for (TInstancedStruct<FInv_ConsumeModifire>& Modifire : ConsumeModifires)
	{
		auto& ModRef = Modifire.GetMutable();
		ModRef.Manifest();
	}
}

void FInv_StrengthModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5, FColor::Green,
		FString::Printf(TEXT("Strength increased by :%f"), GetValue()));
}

void FInv_StrengthModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5, FColor::Red,
		FString::Printf(TEXT("Item Unequip, Strength decreased by :%f"), GetValue()));
}

void FInv_EquipmentFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped)
	{
		return;
	}
	bEquipped = true;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnEquip(PC);
	}
}
void FInv_EquipmentFragment::OnUnequip(APlayerController* PC)
{
	if (!bEquipped)
	{
		return;
	}
	bEquipped = false;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnUnequip(PC);
	}
}

void FInv_EquipmentFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const TInstancedStruct<FInv_EquipModifier>& Modifier : EquipModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FInv_EquipmentFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}

AInv_EquipActor* FInv_EquipmentFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const
{
	if (!IsValid(EquipActorClass) || !IsValid(AttachMesh))
	{
		return nullptr;
	}
	AInv_EquipActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AInv_EquipActor>(EquipActorClass);
	SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttackPoint);

	return SpawnedActor;
}

void FInv_EquipmentFragment::DestroyAttachedActor() const
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}

void FInv_EquipmentFragment::SetEquippedActor(AInv_EquipActor* EquipActor)
{
	EquippedActor = EquipActor;
}

void FInv_ArmorModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5, FColor::Green,
		FString::Printf(TEXT("Item Equip : Armor increased by :%f"), GetValue()));
}

void FInv_ArmorModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5, FColor::Red,
		FString::Printf(TEXT("Item Unequip : Armor Decreased by :%f"), GetValue()));
}

void FInv_DamageModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5, FColor::Green,
		FString::Printf(TEXT("Item Equip : Damage Decreased by :%f"), GetValue()));
}

void FInv_DamageModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5, FColor::Red,
		FString::Printf(TEXT("Item Unequip : Damage Decreased by :%f"), GetValue()));
}
