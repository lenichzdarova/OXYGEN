// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "O2_PlayerStateBase.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API AO2_PlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	
	AO2_PlayerStateBase();
	
public:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return AbilitySystemComponent;}
};
