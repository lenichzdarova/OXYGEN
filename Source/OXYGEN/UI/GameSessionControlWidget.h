// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonSessionSubsystem.h"
#include "GameSessionControlWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class OXYGEN_API UGameSessionControlWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Session")
	void HostSession();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void FindSessions();

	/** Join по индексу из последнего поиска (по умолчанию 0). */
	UFUNCTION(BlueprintCallable, Category = "Session")
	void JoinSession(int32 ResultIndex = 0);

protected:
	void OnSearchFinished(bool bSucceeded, const FText& ErrorMessage);

	UPROPERTY(Transient)
	TObjectPtr<UCommonSession_SearchSessionRequest> ActiveSearchRequest;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCommonSession_SearchResult>> LastSearchResults;	
};
