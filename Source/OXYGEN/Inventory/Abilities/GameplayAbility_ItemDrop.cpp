// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_ItemDrop.h"

#include "InventoryFragment_SpawnDefinition.h"
#include "WorldSpawnedItem.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Inventory_Drop, "Ability.Inventory.ItemDrop", "Drop item ability gameplay tag");

UGameplayAbility_ItemDrop::UGameplayAbility_ItemDrop()
{
	FAbilityTriggerData Trigger;
	Trigger.TriggerTag = TAG_Ability_Inventory_Drop;
	Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(Trigger);

	ActivationOwnedTags.AddTag(TAG_Ability_Inventory_Drop);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGameplayAbility_ItemDrop::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	auto Finish = [&](bool bCancelled)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, bCancelled);
	};

	if (!TriggerEventData)
	{
		Finish(true);
		return;
	}

	FGuid ItemID;
	for (const TSharedPtr<FGameplayAbilityTargetData>& Element : TriggerEventData->TargetData.Data)
	{
		const FGameplayAbilityTargetData* Data = Element.Get();
		if (Data && Data->GetScriptStruct() == FOxygenDropItemTargetData::StaticStruct())
		{
			ItemID = static_cast<const FOxygenDropItemTargetData*>(Data)->ItemId;
			break;
		}
	}

	if (!ItemID.IsValid())
	{
		Finish(true);
		return;
	}

	// Клиент только предсказывает активацию. Спавн и слот — на сервере.
	if (!HasAuthority(&ActivationInfo))
	{
		Finish(false);
		return;
	}

	UInventoryItemRegistry* Registry = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryItemRegistry>();
	const FItemData ItemData = Registry ? Registry->MakeData(ItemID) : FItemData();
	if (!ItemData.IsValid())
	{
		Finish(true);
		return;
	}

	auto Fragment = ItemData.Def->GetDefaultObject<ULyraInventoryItemDefinition>()->FindFragmentByClass(UInventoryFragment_SpawnDefinition::StaticClass());
	const UInventoryFragment_SpawnDefinition* SpawnDefinition = Cast<UInventoryFragment_SpawnDefinition>(Fragment);
		
	if (!SpawnDefinition || !SpawnDefinition->ActorClass)
	{
		Finish(true);
		return;
	}

	const AActor* InstigatorActor = TriggerEventData->Instigator.Get();
	ULyraInventoryManagerComponent* Inventory = InstigatorActor
		? InstigatorActor->FindComponentByClass<ULyraInventoryManagerComponent>()
		: nullptr;
	if (!Inventory || !Inventory->RemoveItem(ItemID))
	{
		Finish(true);
		return;
	}

	FTransform SpawnTransform = InstigatorActor->GetActorTransform();

	AActor* SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnDefinition->ActorClass, SpawnTransform);
	if (SpawnedActor && SpawnedActor->Implements<UWorldSpawnedItem>())
	{
		IWorldSpawnedItem::Execute_InitializeFromItem(SpawnedActor, ItemID, ItemData.Def);
		SpawnedActor->FinishSpawning(SpawnTransform);
		Finish(false);
		return;
	}

	Inventory->AddItem(ItemID, FItemPlacementRequest());
	Finish(true);
}

ULyraInventoryManagerComponent* UGameplayAbility_ItemDrop::GetInventoryManager()
{
	auto Instigator = CurrentEventData.Instigator;
	return Instigator ? Instigator->FindComponentByClass<ULyraInventoryManagerComponent>() : nullptr;
}

void UGameplayAbility_ItemDrop::CalculateSpawnTransfrom(FTransform& OutTransform)
{
	OutTransform = CurrentEventData.Instigator->GetActorTransform();
}
