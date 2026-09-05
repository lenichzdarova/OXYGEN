// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LyraCameraMode.h"
#include "Curves/CurveVector.h"
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
	virtual FVector GetPivotLocation() const override;

	/** Имя сокета, к которому "привязываем" камеру (обычно head / Camera / FP_Camera) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
	FName CameraAttachSocket = TEXT("Camera");

	/** Дополнительный локальный оффсет от сокета (в пространстве меша) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
	FVector CameraOffset = FVector::ZeroVector;

	/** Насколько быстро интерполируем crouch offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
	float CrouchOffsetBlendSpeed = 12.0f;

private:

	void UpdateCrouchOffset(float DeltaTime);
	void SetTargetCrouchOffset(const FVector& NewTargetOffset);

	FVector TargetCrouchOffset = FVector::ZeroVector;
	FVector CurrentCrouchOffset = FVector::ZeroVector;

};
