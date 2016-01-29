// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SHUD.h"
#include "SCharacter.h"
#include "SUsableActor.h"
#include "SPlayerController.h"

ASHUD::ASHUD(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDCenterDotObj(TEXT("Texture2D'/Game/UI/HUD/T_CenterDot_M.T_CenterDot_M'"));
	CenterDotIcon = UCanvas::MakeIcon(HUDCenterDotObj.Object);
}

void ASHUD::DrawHUD()
{
	Super::DrawHUD();
	DrawCenterDot();
}

void ASHUD::DrawCenterDot()
{
	float lCenterX = Canvas->ClipX * 0.5f;
	float lCenterY = Canvas->ClipY * 0.5f;
	float lCenterDotScale = 0.07f;

	ASPlayerController* lOwner = Cast<ASPlayerController>(PlayerOwner);
	if (lOwner)
	{
		ASCharacter* lPawn = Cast<ASCharacter>(lOwner->GetPawn());
		if (lPawn && lPawn->IsAlive())
		{
			ASUsableActor* lUsable = lPawn->GetUsableInView();
			if (lUsable)
				lCenterDotScale *= 1.5f;

			Canvas->SetDrawColor(255, 255, 255, 255);
			Canvas->DrawIcon(CenterDotIcon,
							 lCenterX - CenterDotIcon.UL * lCenterDotScale * 0.5f,
							 lCenterY - CenterDotIcon.VL * lCenterDotScale * 0.5f,
							 lCenterDotScale);
		}
	}
}


