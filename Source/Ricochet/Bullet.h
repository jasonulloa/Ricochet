// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class RICOCHET_API ABullet : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void OnConstruction(const FTransform& Transform) override;

	void SetMyWeapon(AWeapon* owner) { MyWeapon = owner; }

	FVector MyVelocity;

	UFUNCTION()
		void OnOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = Projectile)
		float fSpeed;

	UPROPERTY(EditAnywhere, Category = Projectile)
		float fBounceSpeedLoss;
    
    UPROPERTY(EditDefaultsOnly)
    class USoundBase* ReloadSound;
    
protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;
private:
	AWeapon* MyWeapon;
	int bounceCount;
	float Damage = 50.0f;
};