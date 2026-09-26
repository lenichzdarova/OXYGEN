#include "LyraGameplayAbility_FromEquipment.h"

#include "LyraEquipmentInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGameplayAbility_FromEquipment)

ULyraGameplayAbility_FromEquipment::ULyraGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

ULyraEquipmentInstance* ULyraGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<ULyraEquipmentInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

FGuid ULyraGameplayAbility_FromEquipment::GetAssociatedItemId() const
{
	if (const ULyraEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Equipment->GetItemID();
	}
	return FGuid();
}

#if WITH_EDITOR
EDataValidationResult ULyraGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
	{
		Context.AddError(NSLOCTEXT("Lyra", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif