// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EnemyCharacter.h"
#include "Sound/SoundCue.h"
#include "EnemyWeapon.generated.h"

UCLASS()
class RICOCHET_API AEnemyWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyWeapon();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;
    
    USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }
    AEnemyCharacter* GetMyOwner() { return MyOwner; }
    void SetMyOwner(AEnemyCharacter* Character) { MyOwner = Character; }
    
    void OnStartFire();
    void OnStopFire();
    UAudioComponent* PlayWeaponSound(USoundCue* Sound);
    void WeaponTrace();
    
protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    USkeletalMeshComponent* WeaponMesh;
    
    UPROPERTY(EditDefaultsOnly, Category = Sound)
    class USoundCue* FireLoopSound;
    UPROPERTY(EditDefaultsOnly, Category = Sound)
    class USoundCue* FireFinishSound;
    UPROPERTY(Transient)
    class UAudioComponent* FireAC;
    
    UPROPERTY(EditDefaultsOnly, Category = Effects)
    class UParticleSystem* MuzzleFX;
    UPROPERTY(Transient)
    class UParticleSystemComponent* MuzzlePC;
    
    class AEnemyCharacter* MyOwner;
    
    UPROPERTY(EditAnywhere)
    float FireRate;
    UPROPERTY(EditAnywhere)
    float WeaponRange;
    UPROPERTY(EditDefaultsOnly)
    class UParticleSystem* HitEffect;
    
    UPROPERTY(EditAnywhere, Category = Damage)
    float WeaponDamage = 1.0f;
    
    FTimerHandle WeaponTimer;
	
};
