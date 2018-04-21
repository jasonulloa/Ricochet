// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "EnemyAIController.generated.h"

enum State
{
    Start,
    Patrol,
    Chase,
    Attack,
    Reroute,
    Dead,
};

/**
 * 
 */
UCLASS()
class RICOCHET_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    // set defaults
    AEnemyAIController();
    
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;
    
    virtual void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) override;
    
    void GoToStart();
    void PatrolRoute();
    void AttackPlayer(APawn* Player);
    void StopFiring();
    
    void SetState(State NewState) { mState = NewState; }
    FTimerHandle GetPatrolTimer() { return PatrolTimer; }
    bool GetAttackOnly() { return AttackOnly; }
    
private:
    State mState;
    FTimerHandle PatrolTimer;
    FTimerHandle ShootTimer;
    int WaypointPos;
    bool AttackOnly = false;
};
