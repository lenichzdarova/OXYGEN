// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "CommonSessionSubsystem.h"
#include "FindSessionWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class OXYGEN_API UFindSessionWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	

	UFUNCTION(BlueprintCallable, Category = "Session")
	void FindSessions();

	/** Join по индексу из последнего поиска (по умолчанию 0). */
	UFUNCTION(BlueprintCallable, Category = "Session")
	void JoinSession(UCommonSession_SearchResult* SearchResult);
	
	
	//
	void SetSessionSearchPrivateID(FString InID) {SessionSearchPrivateID = InID;}
	const FString& GetSessionSearchPrivateID() const {return SessionSearchPrivateID;}
	
private:
	FString SessionSearchPrivateID;
	
	UPROPERTY(Transient)
	TObjectPtr<UCommonSession_SearchSessionRequest> ActiveSearchRequest;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCommonSession_SearchResult>> LastSearchResults;		
	UPROPERTY(Transient)
	TWeakObjectPtr<UCommonSession_SearchResult> SelectedResult;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonButtonBase> JoinGameButton;
	
	void OnSearchFinished(bool bSucceeded, const FText& ErrorMessage);	
	void CreateSessionSelectionButtons();
	
	void OnJoinGameButtonClicked();
	

protected:		
	
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	UCommonButtonBase* ConstructSessionButton(UCommonSession_SearchResult* SearchResult);			
	
};
