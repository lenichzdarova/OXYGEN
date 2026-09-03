// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LyraCameraMode.h"
#include "FirstPersonCameraMode.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UFirstPersonCameraMode : public ULyraCameraMode
{
	GENERATED_BODY()
	
	UFirstPersonCameraMode();
	
	protected:

	virtual void UpdateView(float DeltaTime) override;

	void UpdateForTarget(float DeltaTime);	

	virtual void DrawDebug(UCanvas* Canvas) const override;

protected:

	// Curve that defines local-space offsets from the target using the view pitch to evaluate the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "!bUseRuntimeFloatCurves"))
	TObjectPtr<const UCurveVector> TargetOffsetCurve;

	// UE-103986: Live editing of RuntimeFloatCurves during PIE does not work (unlike curve assets).
	// Once that is resolved this will become the default and TargetOffsetCurve will be removed.
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	bool bUseRuntimeFloatCurves;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
	FRuntimeFloatCurve TargetOffsetX;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
	FRuntimeFloatCurve TargetOffsetY;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
	FRuntimeFloatCurve TargetOffsetZ;

	// Alters the speed that a crouch offset is blended in or out
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Third Person")
	float CrouchOffsetBlendMultiplier = 5.0f;	

protected:
	
	void SetTargetCrouchOffset(FVector NewTargetOffset);
	void UpdateCrouchOffset(float DeltaTime);

	FVector InitialCrouchOffset = FVector::ZeroVector;
	FVector TargetCrouchOffset = FVector::ZeroVector;
	float CrouchOffsetBlendPct = 1.0f;
	FVector CurrentCrouchOffset = FVector::ZeroVector;
};
