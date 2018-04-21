// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "MainPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class RICOCHET_API AMainPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_UCLASS_BODY()
    
    virtual void UpdateCamera(float DeltaTime) override;
    
public:
    float NormalFOV;
    float TargetingFOV;
};
