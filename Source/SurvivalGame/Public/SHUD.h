// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "SHUD.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	virtual void DrawHUD() override;
	void DrawCenterDot();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void MessageReceived(const FString& inTextMessage);

public:
	FCanvasIcon CenterDotIcon;
};
