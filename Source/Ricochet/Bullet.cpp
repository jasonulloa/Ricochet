// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "Bullet.h"
#include "EnemyCharacter.h"

// Sets default values
ABullet::ABullet(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	fSpeed = 3000.0f;
	fBounceSpeedLoss = 0.0f;

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlap);   // set up a notification for when this component overlaps something
	//CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ABullet::OnOverlap);     // set up a notification for when this component overlaps something
	RootComponent = CollisionComp;

	bounceCount = 0;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + MyVelocity * DeltaTime, true);
}

void ABullet::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed) {
        // reload bullet
        if (MyWeapon) {
            MyWeapon->ReloadBullet();
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound, MyWeapon->GetActorLocation());
        }
	}
}

void ABullet::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	MyVelocity = this->GetActorForwardVector() * fSpeed;
}

void ABullet::OnOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (bounceCount > 1) {
		this->Destroy();
	}
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL)) {
		AEnemyCharacter* enemy = Cast<AEnemyCharacter>(OtherActor);
		if (enemy) {
			enemy->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
			this->Destroy();
		}
		else {
			bounceCount += 1;
			float dotprod = FVector::DotProduct(SweepResult.ImpactNormal, -MyVelocity);
			FVector newFwd = 2 * dotprod * SweepResult.ImpactNormal + MyVelocity;
			MyVelocity = newFwd;
		}
	}
}