// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "BuildingToolEquipmentInstance.generated.h"

class UOxygenBuildingBlueprintDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreviewChanged, AActor*, PreviewActor);

USTRUCT()
struct FPendingBuildPiece
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> PreviewActor;

	UPROPERTY()
	TObjectPtr<UOxygenBuildingBlueprintDefinition> Definition;
};

UCLASS(BlueprintType)
class OXYGEN_API UBuildingToolEquipmentInstance : public ULyraEquipmentInstance
{
	GENERATED_BODY()

public:
	UBuildingToolEquipmentInstance();

	UPROPERTY(BlueprintAssignable, Category="Building")
	FOnPreviewChanged OnPreviewChanged;

	UPROPERTY(ReplicatedUsing=OnRep_PreviewActor, BlueprintReadOnly, Category="Building")
	TObjectPtr<AActor> PreviewActor;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Building")
	TObjectPtr<UOxygenBuildingBlueprintDefinition> SelectedBlueprint;
	
	UPROPERTY()
	TArray<FPendingBuildPiece> PendingPieces;

	UFUNCTION(BlueprintCallable, Category="Building")
	void SetSelectedBlueprint(UOxygenBuildingBlueprintDefinition* InBlueprint);

	UFUNCTION(BlueprintCallable, Category="Building")
	void ClearSelectedBlueprint();

	virtual void OnUnequipped() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_PreviewActor();

	void BroadcastPreviewChanged();
};
