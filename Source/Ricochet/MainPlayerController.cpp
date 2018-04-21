// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "MainPlayerController.h"
#include "MainPlayerCameraManager.h"


AMainPlayerController::AMainPlayerController(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
    PlayerCameraManagerClass = AMainPlayerCameraManager::StaticClass();
}




