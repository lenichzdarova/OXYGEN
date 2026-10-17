// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BaseManagerSubsystem.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FBaseData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid Id;

	UPROPERTY(BlueprintReadOnly)
	FTransform RootTransform;

	UPROPERTY(BlueprintReadOnly)
	FVector BoundsMin = FVector(FLT_MAX);

	UPROPERTY(BlueprintReadOnly)
	FVector BoundsMax = FVector(-FLT_MAX);

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Pieces;

	FBox GetInfluenceBox(float Padding = 500.f) const
	{
		return FBox(BoundsMin - FVector(Padding), BoundsMax + FVector(Padding));
	}

	bool IsValid() const { return Id.IsValid(); }
};

UCLASS()
class OXYGEN_API UBaseManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Base")
	bool IsLocationInAnyBase(const FVector& Location, float Padding, FGuid& OutBaseId) const;

	UFUNCTION(BlueprintCallable, Category="Base")
	FGuid RegisterPiece(AActor* Piece, const FBox& PieceBounds);

	UFUNCTION(BlueprintCallable, Category="Base")
	void UnregisterPiece(AActor* Piece);
	
	// BaseManagerSubsystem.h
	UFUNCTION(BlueprintCallable, Category="Base")
	FTransform GetPreviewTransform(const FTransform& DesiredTransform) const;

	UFUNCTION(BlueprintCallable, Category="Base")
	FBaseData GetBaseData(const FGuid& BaseId) const;

	UFUNCTION(BlueprintCallable, Category="Base")
	TArray<FBaseData> GetAllBases() const { return Bases; }

	// Округление трансформa до целых
	static FTransform MakeIntegerTransform(const FTransform& InTransform);

private:
	void ExpandBaseBounds(FBaseData& Base, const FBox& PieceBounds);
	int32 FindBaseIndexForBounds(const FBox& Bounds, float Padding) const;
	int32 FindBaseIndexByPiece(AActor* Piece) const;

	UPROPERTY()
	TArray<FBaseData> Bases;
};
