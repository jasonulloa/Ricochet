// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "Engine/TargetPoint.h"
#include "MainCharacter.h"

AEnemyAIController::AEnemyAIController() {
    
}

// Called when the game starts or when spawned
void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    
    // start state machine
    mState = Start;
}

// Called every frame
void AEnemyAIController::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );
    
    // get player pawn
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    // get AI pawn
    APawn* AIPawn = GetPawn();
    
    // check state
    switch (mState) {
        case Start:
            // go to start
            GoToStart();
            break;
        case Chase:
            // check whether to shoot player
            if (PlayerPawn && AIPawn) {
                // get range between ai and player
                FVector PlayerVector = PlayerPawn->GetActorLocation();
                FVector AIVector = AIPawn->GetActorLocation();
                float range = FVector::Dist(PlayerVector, AIVector);
                // check range
                if (range < 800.0f) {
                    // shoot at player
                    SetState(Attack);
                    AttackOnly = true;
                    AttackPlayer(PlayerPawn);
                }
            }
            break;
    }
}

void AEnemyAIController::GoToStart() {
    // get enemy character
    AEnemyCharacter* enemy = Cast<AEnemyCharacter>(GetPawn());
    
    if (enemy) {
        // if waypoints exist
        if(enemy->GetWaypoints().Num() > 0) {
            // set max walk speed
            enemy->GetCharacterMovement()->MaxWalkSpeed = 150.0f;
            
            // move to first waypoint
            MoveToActor(enemy->GetWaypoints()[0]);
            
            // set waypoint position to first one
            WaypointPos = 0;
        }
        
        // start patrolling
        mState = Patrol;
    }
}

void AEnemyAIController::PatrolRoute() {
    // get enemy character
    AEnemyCharacter* enemy = Cast<AEnemyCharacter>(GetPawn());
    
    if (enemy) {
        // set next waypoint position
        if (WaypointPos == enemy->GetWaypoints().Num() - 1) {
            WaypointPos = 0;
        } else {
            WaypointPos++;
        }
        
        // go to next waypoint
        MoveToActor(enemy->GetWaypoints()[WaypointPos]);
    }
}

void AEnemyAIController::AttackPlayer(APawn* Player) {
    AEnemyCharacter* enemy = Cast<AEnemyCharacter>(GetPawn());
    AMainCharacter* MainChar = Cast<AMainCharacter>(Player);
    if (enemy && MainChar) {
        // stop sprinting
        enemy->SetSprinting(false);
        
        // crouch if player is crouched
        if (MainChar->bIsCrouched) {
            enemy->OnCrouch();
            enemy->SetTargeting(false);
        } else {
            // only target if standing
            enemy->SetTargeting(true);
        }
        
        // target and shoot at player
        enemy->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
        enemy->OnStartFire();
        
        // stop after firing burst
        GetWorldTimerManager().SetTimer(ShootTimer, this, &AEnemyAIController::StopFiring, 0.5f);
    }
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) {
    // call base function
    Super::OnMoveCompleted(RequestID, Result);
    
    // wait before continuing patrol
    if (mState == Patrol) {
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyAIController::PatrolRoute, 4.0f);
    }
}

void AEnemyAIController::StopFiring() {
    // stop firing rifle
    AEnemyCharacter* enemy = Cast<AEnemyCharacter>(GetPawn());
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    AMainCharacter* Player = Cast<AMainCharacter>(PlayerPawn);
    if (enemy && Player) {
        // stand up if crouching
        if (enemy->bIsCrouched) {
            enemy->UnCrouch();
        }
        
        // stop target and firing
        enemy->SetTargeting(false);
        enemy->OnStopFire();
        
        // return to regular state machine
        AttackOnly = false;
    }
}




