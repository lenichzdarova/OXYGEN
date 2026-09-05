// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonCameraMode.h"

#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FirstPersonCameraMode)

UFirstPersonCameraMode::UFirstPersonCameraMode()
{
	// Можно сразу задать удобный FOV
	FieldOfView = 100.0f;

	// Ограничения pitch (можно оставить дефолтные из родителя)
	ViewPitchMin = -89.0f;
	ViewPitchMax = 89.0f;
}

void UFirstPersonCameraMode::UpdateView(float DeltaTime)
{
	UpdateCrouchOffset(DeltaTime);

	FVector PivotLocation = GetPivotLocation() + CurrentCrouchOffset;
	FRotator PivotRotation = GetPivotRotation();

	// Клампим pitch
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = PivotRotation;
	View.FieldOfView = FieldOfView;
}

FVector UFirstPersonCameraMode::GetPivotLocation() const
{	
	const AActor* TargetActor = GetTargetActor();
	if (!TargetActor)
	{
		return FVector::ZeroVector;
	}	
	if (const ACharacter* Character = Cast<ACharacter>(TargetActor))
	{		
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			if (Mesh->DoesSocketExist(CameraAttachSocket))
			{				
				const FTransform SocketTransform = Mesh->GetSocketTransform(CameraAttachSocket, RTS_World);
				
				// Применяем локальный оффсет в пространстве сокета
				return SocketTransform.TransformPosition(CameraOffset);
			}

			// Fallback — если сокета нет
			return Mesh->GetComponentLocation();
		}
	}

	// Последний fallback
	if (const APawn* Pawn = Cast<APawn>(TargetActor))
	{
		return Pawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

void UFirstPersonCameraMode::UpdateCrouchOffset(float DeltaTime)
{
	if (const ACharacter* Character = Cast<ACharacter>(GetTargetActor()))
	{
		if (Character->bIsCrouched)
		{
			const ACharacter* CDO = Character->GetClass()->GetDefaultObject<ACharacter>();
			const float HeightDiff = CDO->CrouchedEyeHeight - CDO->BaseEyeHeight;
			SetTargetCrouchOffset(FVector(0.f, 0.f, HeightDiff));
		}
		else
		{
			SetTargetCrouchOffset(FVector::ZeroVector);
		}
	}
	else
	{
		SetTargetCrouchOffset(FVector::ZeroVector);
	}

	// Плавная интерполяция
	CurrentCrouchOffset = FMath::VInterpTo(CurrentCrouchOffset, TargetCrouchOffset, DeltaTime, CrouchOffsetBlendSpeed);
}

void UFirstPersonCameraMode::SetTargetCrouchOffset(const FVector& NewTargetOffset)
{
	TargetCrouchOffset = NewTargetOffset;
}