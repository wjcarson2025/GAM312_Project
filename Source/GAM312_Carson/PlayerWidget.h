// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAM312_CARSON_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBars(float hlth, float hngr, float stmna);

};
