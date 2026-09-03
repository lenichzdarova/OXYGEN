// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonCameraMode.h"

#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"

UFirstPersonCameraMode::UFirstPersonCameraMode()
{
	TargetOffsetCurve = nullptr;
}

void UFirstPersonCameraMode::UpdateView(float DeltaTime)
{
	UpdateForTarget(DeltaTime);
	UpdateCrouchOffset(DeltaTime);

	FVector PivotLocation = GetPivotLocation() + CurrentCrouchOffset;
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// Apply third person offset using pitch.
	if (!bUseRuntimeFloatCurves)
	{
		if (TargetOffsetCurve)
		{
			const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
			View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
		}
	}
	else
	{
		FVector TargetOffset(0.0f);

		TargetOffset.X = TargetOffsetX.GetRichCurveConst()->Eval(PivotRotation.Pitch);
		TargetOffset.Y = TargetOffsetY.GetRichCurveConst()->Eval(PivotRotation.Pitch);
		TargetOffset.Z = TargetOffsetZ.GetRichCurveConst()->Eval(PivotRotation.Pitch);

		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}
}

void UFirstPersonCameraMode::UpdateForTarget(float DeltaTime)
{

	if (const ACharacter* TargetCharacter = Cast<ACharacter>(GetTargetActor()))
	{
		if (TargetCharacter->IsCrouched())
		{
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
			const float CrouchedHeightAdjustment = TargetCharacterCDO->CrouchedEyeHeight - TargetCharacterCDO->BaseEyeHeight;

			SetTargetCrouchOffset(FVector(0.f, 0.f, CrouchedHeightAdjustment));

			return;
		}
	}

	SetTargetCrouchOffset(FVector::ZeroVector);
}

void UFirstPersonCameraMode::DrawDebug(UCanvas* Canvas) const
{
	Super::DrawDebug(Canvas);
}



void UFirstPersonCameraMode::SetTargetCrouchOffset(FVector NewTargetOffset)
{
	CrouchOffsetBlendPct = 0.0f;
	InitialCrouchOffset = CurrentCrouchOffset;
	TargetCrouchOffset = NewTargetOffset;
}


void UFirstPersonCameraMode::UpdateCrouchOffset(float DeltaTime)
{
	if (CrouchOffsetBlendPct < 1.0f)
	{
		CrouchOffsetBlendPct = FMath::Min(CrouchOffsetBlendPct + DeltaTime * CrouchOffsetBlendMultiplier, 1.0f);
		CurrentCrouchOffset = FMath::InterpEaseInOut(InitialCrouchOffset, TargetCrouchOffset, CrouchOffsetBlendPct, 1.0f);
	}
	else
	{
		CurrentCrouchOffset = TargetCrouchOffset;
		CrouchOffsetBlendPct = 1.0f;
	}
}