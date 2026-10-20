// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HabitatConstructionComponent.h"
#include "GameFramework/Actor.h"
#include "HabitatActorBase.generated.h"

UCLASS(Abstract)
class OXYGEN_API AHabitatActorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHabitatActorBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Building")
	TObjectPtr<UHabitatConstructionComponent> ConstructionComponent;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category="Building")
	void HandleConstructionStarted();
	virtual void HandleConstructionStarted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Building")
	void HandleProgressChanged(float NewProgress);
	virtual void HandleProgressChanged_Implementation(float NewProgress);

	UFUNCTION(BlueprintNativeEvent, Category="Building")
	void HandleConstructionCompleted();
	virtual void HandleConstructionCompleted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Building")
	void HandleConstructionCancelled();
	virtual void HandleConstructionCancelled_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Building")
	void HandleDeconstructionStarted();
	virtual void HandleDeconstructionStarted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Building")
	void HandleDeconstructionCompleted();
	virtual void HandleDeconstructionCompleted_Implementation();
};

