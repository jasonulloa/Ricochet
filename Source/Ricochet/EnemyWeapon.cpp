// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "EnemyWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "MainCharacter.h"

// Sets default values
AEnemyWeapon::AEnemyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;
    
    FireRate = 0.1f;
    WeaponRange = 10000.0f;
}

// Called when the game starts or when spawned
void AEnemyWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

UAudioComponent* AEnemyWeapon::PlayWeaponSound(USoundCue* Sound) {
    UAudioComponent* AC = NULL;
    if (Sound) {
        AC = UGameplayStatics::SpawnSoundAttached(Sound, RootComponent);
    }
    return AC;
}

void AEnemyWeapon::OnStartFire() {
    // fire rate timer
    GetWorldTimerManager().SetTimer(WeaponTimer, this, &AEnemyWeapon::WeaponTrace, FireRate, true);
    
    // play firing sound
    FireAC = PlayWeaponSound(FireLoopSound);
    
    // start muzzle flash
    MuzzlePC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, WeaponMesh, "MuzzleFlashSocket");
}

void AEnemyWeapon::OnStopFire() {
    // fire rate timer
    GetWorldTimerManager().ClearTimer(WeaponTimer);
    
    // stop firing sound
    if (FireAC) {
        FireAC->Stop();
    }
    
    // play stopped sound
    PlayWeaponSound(FireFinishSound);
    
    // stop muzzle flash
    if (MuzzlePC) {
        MuzzlePC->DeactivateSystem();
    }
}

void AEnemyWeapon::WeaponTrace() {
    static FName WeaponFireTag = FName(TEXT("WeaponTrace"));
    static FName MuzzleSocket = FName(TEXT("MuzzleFlashSocket"));
    
    // Start from the muzzle's position
    FVector StartPos = WeaponMesh->GetSocketLocation(MuzzleSocket);
    // Get forward vector of MyPawn
    FVector Forward = MyOwner->GetActorForwardVector();
    // Calculate end position
    FVector EndPos = StartPos + (Forward * WeaponRange);
    
    // Perform trace to retrieve hit info
    FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
    TraceParams.bTraceAsyncScene = true;
    TraceParams.bReturnPhysicalMaterial = true;
    
    // This fires the ray and checks against all objects w/ collision
    FHitResult Hit(ForceInit);
    GetWorld()->LineTraceSingleByObjectType(Hit, StartPos, EndPos, FCollisionObjectQueryParams::AllObjects, TraceParams);
    // Did this hit anything?
    if (Hit.bBlockingHit)
    {
        // spawn particle effect
        UGameplayStatics::SpawnEmitterAtLocation(this, HitEffect, Hit.ImpactPoint);
        
        // player takes damage if hit
        AMainCharacter* Player = Cast<AMainCharacter>(Hit.GetActor());
        if (Player) {
            Player->TakeDamage(WeaponDamage, FDamageEvent(), GetInstigatorController(), this);
        }
    }
}

