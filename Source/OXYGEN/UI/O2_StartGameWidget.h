// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "CommonUserTypes.h"
#include "O2_SaveManager.h"
#include "Components/VerticalBox.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "O2_StartGameWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class OXYGEN_API UO2_StartGameWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
	
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetStack(UCommonActivatableWidgetStack* InWidgetStack) {WidgetStack = InWidgetStack;};
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> CreateSavedGameButtonClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> CreateNewGameButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UVerticalBox> ButtonContainer;	
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCommonActivatableWidgetStack> WidgetStack;
	
	// old games
	void CreateButtonsForSavedWorlds();
	void GetSaveGameNames(TArray<FString>& SlotNames);
	
	UFUNCTION()
	void OnCreateSavedGameButtonClicked(UO2_SaveGame* InSavedGame);	
	
	// new game
	UFUNCTION()
	void OnCreateNewGameButtonClicked();
	void OnSessionCreateComplete(const FOnlineResultInformation& Result);	
	
};
