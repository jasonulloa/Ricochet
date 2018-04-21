// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "Weapon.h"
#include "BulletHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "MainCharacter.h"

ABulletHUD::ABulletHUD(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	static ConstructorHelpers::FObjectFinder<UFont>HUDFontObj(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = HUDFontObj.Object;
}

void ABulletHUD::DrawHUD() {
	Super::DrawHUD();
	FVector2D ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);

	APlayerController* pc = GetOwningPlayerController();
	mMPC = Cast<AMainPlayerController>(pc);
	AMainCharacter* player = Cast<AMainCharacter>(mMPC->GetPawn());
    
    FString label = FString("Rifle: ");
    DrawText(label, FColor::White, 50, Canvas->SizeY - 100, HUDFont);
    
	int bullets = player->GetMyWeapon()->GetAmmoCount();
    if (bullets > 0) {
        DrawText("Charged", FColor::Green, 107, Canvas->SizeY - 100, HUDFont);
    } else {
        DrawText("Drained", FColor::Red, 107, Canvas->SizeY - 100, HUDFont);
    }
}