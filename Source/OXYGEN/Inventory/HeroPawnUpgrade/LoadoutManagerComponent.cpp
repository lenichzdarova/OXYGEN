#include "LoadoutManagerComponent.h"

#include "Inventory/LyraInventoryItemInstance.h"
#include "Inventory/InventoryFragment_LoadoutItem.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

ULoadoutManagerComponent::ULoadoutManagerComponent(const FObjectInitializer& ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}
