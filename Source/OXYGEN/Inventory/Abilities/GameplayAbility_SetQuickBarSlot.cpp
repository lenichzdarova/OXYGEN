// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_SetQuickBarSlot.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Inventory/Equipment/LyraQuickBarComponent.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbility_SetQuickBarSlot)

// Теги инпутов для слотов QuickBar
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_QuickBar_Slot1, "InputTag.QuickBar.Slot1", "Select QuickBar slot 1");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_QuickBar_Slot2, "InputTag.QuickBar.Slot2", "Select QuickBar slot 2");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_QuickBar_Slot3, "InputTag.QuickBar.Slot3", "Select QuickBar slot 3");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_QuickBar_Slot4, "InputTag.QuickBar.Slot4", "Select QuickBar slot 4");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_QuickBar_Slot5, "InputTag.QuickBar.Slot5", "Select QuickBar slot 5");

UGameplayAbility_SetQuickBarSlot::UGameplayAbility_SetQuickBarSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationPolicy = ELyraAbilityActivationPolicy::OnInputTriggered;
}

void UGameplayAbility_SetQuickBarSlot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	int32 SlotIndex = INDEX_NONE;

	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		SlotIndex = GetSlotIndexFromInputTags(Spec->GetDynamicSpecSourceTags());
	}

	if (SlotIndex != INDEX_NONE)
	{
		if (APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
		{
			if (ULyraQuickBarComponent* QuickBar = Pawn->FindComponentByClass<ULyraQuickBarComponent>())
			{
				QuickBar->SetActiveSlotIndex(SlotIndex);
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

int32 UGameplayAbility_SetQuickBarSlot::GetSlotIndexFromInputTags(const FGameplayTagContainer& InputTags) const
{
	if (InputTags.HasTagExact(InputTag_QuickBar_Slot1)) return 0;
	if (InputTags.HasTagExact(InputTag_QuickBar_Slot2)) return 1;
	if (InputTags.HasTagExact(InputTag_QuickBar_Slot3)) return 2;
	if (InputTags.HasTagExact(InputTag_QuickBar_Slot4)) return 3;
	if (InputTags.HasTagExact(InputTag_QuickBar_Slot5)) return 4;

	return INDEX_NONE;
}