// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingLegComponent.h"

#include "Net/UnrealNetwork.h"

UBuildingLegComponent::UBuildingLegComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
}

void UBuildingLegComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBuildingLegComponent, LegScale);
}

void UBuildingLegComponent::OnRegister()
{
	Super::OnRegister();

	auto CreateMesh = [this](const FName& Name) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this, Name);
		Mesh->SetupAttachment(this);
		Mesh->RegisterComponent();
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return Mesh;
	};

	if (!TopMesh)    TopMesh    = CreateMesh(TEXT("TopMesh"));
	if (!MidMesh)    MidMesh    = CreateMesh(TEXT("MidMesh"));
	if (!BottomMesh) BottomMesh = CreateMesh(TEXT("BottomMesh"));

	if (TopMeshAsset)
	{
		TopMesh->SetStaticMesh(TopMeshAsset);
		TopMesh->SetMaterial(0,TopMeshMaterial);
	}    
	if (MidMeshAsset)
	{
		MidMesh->SetStaticMesh(MidMeshAsset);
		MidMesh->SetMaterial(0,MidMeshMaterial);
	}   
	if (BottomMeshAsset)
	{
		BottomMesh->SetStaticMesh(BottomMeshAsset);
		BottomMesh->SetMaterial(0,BottomMeshMaterial);
	}
	
	BottomMesh->AttachToComponent(MidMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Foot");
}


void UBuildingLegComponent::UpdateLeg()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (!GetWorld() || !MidMesh || !BottomMesh)
		return;

	const FVector Start = GetComponentLocation();
	const FVector End = Start - FVector(0.f, 0.f, MaxLength);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel2, Params))
	{
		SetVisibility(false, true);
		return;
	}

	SetVisibility(true, true);

	const float Distance = Hit.Distance;
	LegScale = FMath::Max(Distance / DefaultMidLength, 0.01f);
	OnRep_LegScale();
	
	// Середина скейлится вниз
	

	// Низ ставится в точку хита
	//BottomMesh->SetWorldLocation(Hit.Location);
	//BottomMesh->SetWorldRotation(FRotator::ZeroRotator);
}


void UBuildingLegComponent::OnRep_LegScale()
{
	MidMesh->SetRelativeScale3D(FVector(1.f, 1.f, LegScale));
	BottomMesh->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
}

