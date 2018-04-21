// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "MainCharacterMovementComponent.h"
#include "MainCharacter.h"

float UMainCharacterMovementComponent::GetMaxSpeed() const
{
    float MaxSpeed = Super::GetMaxSpeed();
    
    const AMainCharacter* CharOwner = Cast<AMainCharacter>(PawnOwner);
    if (CharOwner) {
        if (CharOwner->IsTargeting() && !CharOwner->GetMovementComponent()->IsCrouching()) {
            MaxSpeed *= CharOwner->GetTargetingSpeedModifier();
        }
        else if (CharOwner->IsSprinting()) {
            MaxSpeed *= CharOwner->GetSprintingSpeedModifier();
        }
    }
    
    return MaxSpeed;
}


