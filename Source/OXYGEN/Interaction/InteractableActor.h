// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorInteractableComponent.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

UCLASS(Blueprintable)
class OXYGEN_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Root scene component")
	USceneComponent* DefaultRootComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UActorInteractableComponent* InteractableComponent;
	
	virtual UActorInteractableComponent* GetInteractableComponent_Implementation() override {return InteractableComponent;};
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnInteract(AActor* InInstigator, AController* InController);
	UFUNCTION(BlueprintNativeEvent)
	void OnStopInteract(AActor* InInstigator, AController* InController);
};
