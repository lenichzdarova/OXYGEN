// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PersistentObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPersistentObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OXYGEN_API IPersistentObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/** Вызывается системой сохранения после восстановления состояния актора */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Persistence")
	void OnAfterLoad();
	virtual void OnAfterLoad_Implementation() {}
};
