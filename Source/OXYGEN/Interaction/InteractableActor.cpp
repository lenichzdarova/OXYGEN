// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"


// Sets default values
AInteractableActor::AInteractableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRootComponent;
	
	InteractableComponent = CreateDefaultSubobject<UActorInteractableComponent>("InteractableComponent");
	InteractableComponent->SetupAttachment(RootComponent);
	InteractableComponent->OnInteractionStarted.AddDynamic(this, &AInteractableActor::OnInteract);
	InteractableComponent->OnInteractionEnded.AddDynamic(this, &AInteractableActor::OnStopInteract);
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableActor::OnStopInteract_Implementation(AActor* InInstigator, AController* InControllerr)
{
}

void AInteractableActor::OnInteract_Implementation(AActor* InInstigator, AController* InController)
{	
}

