// AbilityTask_WaitInputPressByTag.cpp
#include "AbilityTask_WaitInputPressByTag.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"

UAbilityTask_WaitInputPressByTag* UAbilityTask_WaitInputPressByTag::WaitInputPressByTag(UGameplayAbility* OwningAbility, FGameplayTag InputTag)
{
    UAbilityTask_WaitInputPressByTag* Task = NewAbilityTask<UAbilityTask_WaitInputPressByTag>(OwningAbility);
    Task->WatchedTag = InputTag;
    return Task;
}

void UAbilityTask_WaitInputPressByTag::Activate()
{
    Super::Activate();

    if (!Ability || !AbilitySystemComponent.IsValid() || !WatchedTag.IsValid())
    {
        EndTask();
        return;
    }

    // Слушаем стандартное InputPressed событие абилки
    AbilitySystemComponent->AbilityReplicatedEventDelegate(
        EAbilityGenericReplicatedEvent::InputPressed,
        GetAbilitySpecHandle(),
        GetActivationPredictionKey()
    ).AddUObject(this, &UAbilityTask_WaitInputPressByTag::OnInputPressed);

    // Дополнительно: если хочешь ловить и уже зажатый
    if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(AbilitySystemComponent.Get()))
    {
        // Можно подписаться на свой делегат, если добавишь его в LyraASC
    }
}

void UAbilityTask_WaitInputPressByTag::OnInputPressed()
{
    if (!Ability || !AbilitySystemComponent.IsValid())
        return;

    // Проверяем, что у спека этой абилки есть нужный тег
    // и что именно он сейчас "нажат"
    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(GetAbilitySpecHandle());
    if (!Spec)
        return;

    if (Spec->GetDynamicSpecSourceTags().HasTagExact(WatchedTag))
    {
        // Дополнительная проверка: был ли этот тег среди только что нажатых
        // В чистом Lyra это можно сделать, если хранить последний нажатый тег на ASC
        // или если абилка имеет только один тег на инпут.
        // Для простоты сейчас считаем, что совпадение тега достаточно,
        // если абилка привязана только к этому инпуту.
        // Если нужно точнее — см. ниже.

        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnPress.Broadcast();
        }
        EndTask();
    }
}

void UAbilityTask_WaitInputPressByTag::OnDestroy(bool bInOwnerFinished)
{
    if (AbilitySystemComponent.IsValid())
    {
        AbilitySystemComponent->AbilityReplicatedEventDelegate(
            EAbilityGenericReplicatedEvent::InputPressed,
            GetAbilitySpecHandle(),
            GetActivationPredictionKey()
        ).RemoveAll(this);
    }
    Super::OnDestroy(bInOwnerFinished);
}