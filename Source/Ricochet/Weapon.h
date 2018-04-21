// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainCharacter.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class RICOCHET_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
    // Sets default values for this actor's properties
    AWeapon();
    
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;
    
    USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }
    void OnStartAim();
    void OnStopAim();
	void FireBullet();
	void ReloadBullet() { ammo += 1; }
    void SetMyOwner(AMainCharacter* owner) { MyOwner = owner; }
    AMainCharacter* GetMyOwner() { return MyOwner; }
    int GetAmmoCount() { return ammo; }
    
    void Bounce();
    void WeaponTrace(FVector start, FVector fwd, int count);
    UParticleSystemComponent* PlayBeamEffect(UParticleSystem* Effects);
    
protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    USkeletalMeshComponent* WeaponMesh;
    
    UPROPERTY(EditDefaultsOnly, Category = Effects)
    class UParticleSystem* BeamFX;
    
    UPROPERTY(Transient)
    class UParticleSystemComponent* BeamPSC;
    
    UPROPERTY(EditAnywhere)
    float WeaponRange;
    
    UPROPERTY(EditAnywhere)
    float FireRate;
    
    UPROPERTY(EditDefaultsOnly)
    class USoundBase* NoAmmoSound;
    
    UPROPERTY(EditDefaultsOnly)
    class USoundBase* BulletSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABullet> bullet;
    
private:
    class ABulletHUD* hud;
    AMainCharacter* MyOwner;
    FTimerHandle WeaponTimer;
    float Damage = 50.0f;
	int ammo = 1;
};
