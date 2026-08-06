// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSessionControlWidget.h"

#include "O2_SessionSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameSessionControlWidget)

void UGameSessionControlWidget::HostSession()
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

	UCommonSession_HostSessionRequest* Request = Sessions->CreateOnlineHostSessionRequest();
	Sessions->HostSession(PC, Request);
}

void UGameSessionControlWidget::FindSessions()
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
	ActiveSearchRequest->OnSearchFinished.AddUObject(this, &UGameSessionControlWidget::OnSearchFinished);
	Sessions->FindSessions(PC, ActiveSearchRequest);
}

void UGameSessionControlWidget::OnSearchFinished(bool bSucceeded, const FText& ErrorMessage)
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
	}	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt(LastSearchResults.Num()));
}

void UGameSessionControlWidget::JoinSession(int32 ResultIndex)
{
	UGameInstance* GI = GetGameInstance();
	APlayerController* PC = GetOwningPlayer();
	if (!GI || !PC)
	{
		return;
	}

	if (!LastSearchResults.IsValidIndex(ResultIndex) || !LastSearchResults[ResultIndex])
	{
		return;
	}

	UO2_SessionSubsystem* Sessions = GI->GetSubsystem<UO2_SessionSubsystem>();
	if (!Sessions)
	{
		return;
	}

	Sessions->JoinSession(PC, LastSearchResults[ResultIndex]);
}