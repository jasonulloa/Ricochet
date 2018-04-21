// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainPlayerController.h"
#include "GameFramework/HUD.h"
#include "BulletHUD.generated.h"

/**
 * 
 */
UCLASS()
class RICOCHET_API ABulletHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = BulletHUD)
	UFont* HUDFont;
	
	UPROPERTY(EditAnywhere, Category = BulletHUD)
	float DefaultFontScale;

	virtual void DrawHUD() override;

private:
	AMainPlayerController* mMPC;
};
