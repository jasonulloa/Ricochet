// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class RICOCHET_API AEnemyCharacter : public ACharacter
{
    GENERATED_UCLASS_BODY()
    
public:
    // Sets default values for this character's properties
    AEnemyCharacter();
    
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;
    
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    
    virtual float TakeDamage(float Damage, const struct FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;
    
    TArray<class ATargetPoint*>& GetWaypoints() { return EnemyWaypoints; }
    
    // start + stop attack
    void OnStartFire();
    void OnStopFire();
    // destory enemy character
    void DestroyEnemy();
    bool IsDead();
    
    // pursue player
    void ChasePlayer();
    
    // enemy movement
    void SetTargeting(bool NewTargeting);
    
    /* Is player aiming down sights */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    bool IsTargeting() const;
    
    UPROPERTY(Transient, Replicated)
    bool bIsTargeting;
    
    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetingSpeedModifier;
    
    void OnCrouch();
    
    /* Character wants to run, checked during Tick to see if allowed */
    UPROPERTY(Transient, Replicated)
    bool bWantsToRun;
    
    /* Client/local call to update sprint state  */
    void SetSprinting(bool NewSprinting);
    
    UFUNCTION(BlueprintCallable, Category = Movement)
    bool IsSprinting() const;
        
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintingSpeedModifier;
    
protected:
    UPROPERTY(EditAnywhere)
    TArray<class ATargetPoint*> EnemyWaypoints;
    
    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage* DeathAnim;
    
    UPROPERTY(EditDefaultsOnly)
    class USoundBase* AlertSound;
    
    UPROPERTY(EditDefaultsOnly)
    class USoundBase* DeathSound;
    
    UPROPERTY(VisibleAnywhere, Category = "AI")
    class UPawnSensingComponent* PawnSensingComp;
    
    UFUNCTION()
    void OnSenseVisual(APawn* Pawn);
    
    UFUNCTION()
    void OnSenseNoise(APawn* PawnInstigator, const FVector& Location, float Volume);
    
    UPROPERTY(EditAnywhere, Category = Weapon)
    TSubclassOf<class AEnemyWeapon> WeaponClass;
    
private:
    bool PlayerSensed = false;
    float TimeSinceSeen = 0.0f;
    float TimeSinceHeard = 0.0f;
    FTimerHandle DeathTimer;
    FTimerHandle AttackTimer;
    FTimerHandle ResetTimer;
    FTimerHandle AlertTimer;
    FTimerHandle ChaseTimer;
    bool bIsDead = false;
    class AEnemyWeapon* MyWeapon;
	
};
