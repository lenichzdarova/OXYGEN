// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"

#include "OXYGEN/AbilitySystem/GameAbilitySystemComponent.h"

APlayerStateBase::APlayerStateBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGameAbilitySystemComponent>("AbilitySystemComponent");
}
