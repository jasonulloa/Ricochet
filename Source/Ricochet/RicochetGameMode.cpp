// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Ricochet.h"
#include "RicochetGameMode.h"
#include "RicochetCharacter.h"
#include "MainPlayerController.h"
#include "BulletHUD.h"

ARicochetGameMode::ARicochetGameMode()
{
    // set default player controller
    PlayerControllerClass = AMainPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MainCharacter/MainCharacterBP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = ABulletHUD::StaticClass();
}
