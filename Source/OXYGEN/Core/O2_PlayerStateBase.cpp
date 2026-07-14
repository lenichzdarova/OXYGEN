// Fill out your copyright notice in the Description page of Project Settings.


#include "O2_PlayerStateBase.h"

#include "OXYGEN/AbilitySystem/O2_AbilitySystemComponent.h"

AO2_PlayerStateBase::AO2_PlayerStateBase()
{
	CreateDefaultSubobject<UO2_AbilitySystemComponent>("AbilitySystemComponent");
}
