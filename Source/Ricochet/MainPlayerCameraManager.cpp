// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "MainPlayerCameraManager.h"
#include "MainCharacter.h"


AMainPlayerCameraManager::AMainPlayerCameraManager(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) {
    NormalFOV = 75.0f;
    TargetingFOV = 45.0f;
    
    ViewPitchMin = -80.0f;
    ViewPitchMax = 87.0f;
    bAlwaysApplyModifiers = true;
}


void AMainPlayerCameraManager::UpdateCamera(float DeltaTime) {
    AMainCharacter* MyPawn;
    
    if (PCOwner) {
        MyPawn = Cast<AMainCharacter>(PCOwner->GetPawn());
    } else {
        MyPawn = NULL;
    }
    
    if (MyPawn) {
        const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
        DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
        SetFOV(DefaultFOV);
    }
    
    Super::UpdateCamera(DeltaTime);
}



