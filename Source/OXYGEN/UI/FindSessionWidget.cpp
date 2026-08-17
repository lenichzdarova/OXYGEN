// Fill out your copyright notice in the Description page of Project Settings.


#include "FindSessionWidget.h"

#include "O2_SessionSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

void UFindSessionWidget::FindSessions()
{
	UGameInstance* GI = GetGameInstance();
	APlayerController* PC = GetOwningPlayer();
	if (!GI || !PC)
	{
		return;
	}

	UO2_SessionSubsystem* Sessions = GI->GetSubsystem<UO2_SessionSubsystem>();
	if (!Sessions)
	{
		return;
	}

	if (ActiveSearchRequest)
	{
		ActiveSearchRequest->OnSearchFinished.RemoveAll(this);
		ActiveSearchRequest = nullptr;
	}

	LastSearchResults.Reset();

	ActiveSearchRequest = Sessions->CreateOnlineSearchSessionRequest();
	ActiveSearchRequest->OnSearchFinished.AddUObject(this, &UFindSessionWidget::OnSearchFinished);
	Sessions->FindSessions(PC, ActiveSearchRequest);
}

void UFindSessionWidget::OnSearchFinished(bool bSucceeded, const FText& ErrorMessage)
{
	LastSearchResults.Reset();

	if (ActiveSearchRequest)
	{
		if (bSucceeded)
		{
			LastSearchResults = ActiveSearchRequest->Results;
		}
		ActiveSearchRequest->OnSearchFinished.RemoveAll(this);
		ActiveSearchRequest = nullptr;
		CreateSessionSelectionButtons();
	}		
}

void UFindSessionWidget::CreateSessionSelectionButtons()
{
	for (auto SearchResult : LastSearchResults)
	{
		auto Button = ConstructSessionButton(SearchResult);
		Button->OnClicked().AddLambda([this, SearchResult]()
		{
			SelectedResult = SearchResult;
		});
	}
}

void UFindSessionWidget::OnJoinGameButtonClicked()
{
	if (SelectedResult.Get())
	{
		JoinSession(SelectedResult.Get());
	}	
}

void UFindSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	JoinGameButton->OnClicked().AddUObject(this, &UFindSessionWidget::OnJoinGameButtonClicked);
}

void UFindSessionWidget::JoinSession(UCommonSession_SearchResult* SearchResult)
{
	UGameInstance* GI = GetGameInstance();
	APlayerController* PC = GetOwningPlayer();
	if (!GI || !PC)
	{
		return;
	}

	if (!SearchResult)
	{
		return;
	}

	UO2_SessionSubsystem* Sessions = GI->GetSubsystem<UO2_SessionSubsystem>();
	if (!Sessions)
	{
		return;
	}
	Sessions->JoinSession(PC, SearchResult);
}
