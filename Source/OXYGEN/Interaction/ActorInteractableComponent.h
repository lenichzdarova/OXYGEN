// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ActorInteractableComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UActorInteractableComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActorInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY(EditAnywhere)
	bool bShowWidget = true;
	UPROPERTY(EditAnywhere)
	bool bInteractionEnabled = true;
	
public:
	void Interact(AActor* Instigator, AController* Controller);
	void StopInteract(AActor* Instigator, AController* Controller);
	void Highlight(bool bEnable);
	UFUNCTION(BlueprintCallable)
	void EnableInteraction() {bInteractionEnabled = true;}
	UFUNCTION(BlueprintCallable)
	void DisableInteraction() {bInteractionEnabled = false;}
	void SetNeedWidgetToBeShown(bool NeedToShow) {bShowWidget = NeedToShow;}
	bool GetNeedWidgetToBeShown() const {return bShowWidget;}
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStarted, AActor*, Instigator, AController*, PController);
	UPROPERTY(BlueprintAssignable)
	FOnInteractionStarted OnInteractionStarted;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnded, AActor*, Instigator, AController*, Controller);
	UPROPERTY(BlueprintAssignable)
	FOnInteractionEnded OnInteractionEnded;	
};
