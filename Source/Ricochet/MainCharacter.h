// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class RICOCHET_API AMainCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()
    
    UPawnNoiseEmitterComponent* NoiseEmitterComp;
    
    UFUNCTION(BlueprintCallable, Category = "AI")
    void EmitSound(float Strength);

public:
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
    
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    void OnStartAim();
    void OnStopAim();
	void FireBullet();

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

    virtual void MoveForward(float Val);
    
    virtual void MoveRight(float Val);
    
    void OnCrouch();
    
    void OnStartSprinting();
    void OnStopSprinting();
    
    UPROPERTY(Transient, Replicated)
    bool bWantsToRun;
    
    void SetSprinting(bool NewSprinting);
    
    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool IsSprinting() const;
    
    float GetSprintingSpeedModifier() const;
    
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintingSpeedModifier;
    
    void OnTargeting();
    
    void SetTargeting(bool NewTargeting);
    
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    bool IsTargeting() const;
    
    float GetTargetingSpeedModifier() const;
    
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    FRotator GetAimOffsets() const;
    
    UPROPERTY(Transient, Replicated)
    bool bIsTargeting;
    
    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetingSpeedModifier;
    
    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetHealth() const;
    
    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool IsAlive() const;
    
    UPROPERTY(EditDefaultsOnly, Category = "Stats", Replicated)
    float Health;
    
    UPROPERTY(EditAnywhere, Category = Weapon)
    TSubclassOf<class AWeapon> WeaponClass;
    
    virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
    
    // dont get up after death
    void StayDown();
    // reload level
    void RestartLevel();
    
    AWeapon* GetMyWeapon() { return MyWeapon; }
    
protected:
    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage* DeathAnim;
    
    UPROPERTY(EditDefaultsOnly)
    class USoundBase* DeathSound;
private:
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    USpringArmComponent* CameraBoomComp;
    
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    UCameraComponent* CameraComp;
    
    FTimerHandle DeathTimer;
    FTimerHandle RestartTimer;
    
    class AWeapon* MyWeapon;
};
