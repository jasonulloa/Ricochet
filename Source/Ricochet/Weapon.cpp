// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "Weapon.h"
#include "Bullet.h"
#include "BulletHUD.h"
#include "EnemyCharacter.h"


// Sets default values
AWeapon::AWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;
    FireRate = 0.1f;
    WeaponRange = 10000.0f;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
    Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );
    
}

void AWeapon::Bounce() {
    static FName WeaponFireTag = FName(TEXT("WeaponTrace"));
    static FName MuzzleSocket = FName(TEXT("MuzzleFlashSocket"));
    
    // Start from the muzzle's position
    FVector StartPos = WeaponMesh->GetSocketLocation(MuzzleSocket);
    // Get forward vector of MyPawn
    FVector Forward = MyOwner->GetController()->GetActorForwardVector();
    Forward.Normalize();
    
    UGameplayStatics::SpawnEmitterAtLocation(this, BeamFX, StartPos, Forward.Rotation());
    
    WeaponTrace(StartPos, Forward, 0);
}

void AWeapon::WeaponTrace(FVector start, FVector fwd, int count) {
    if (count > 1) {
        return;
    }
    
    static FName WeaponFireTag = FName(TEXT("WeaponTrace"));
    
    // Calculate end position
    FVector EndPos = start + fwd * WeaponRange;
    
    // Perform trace to retrieve hit info
    FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
    TraceParams.bTraceAsyncScene = true;
    TraceParams.bReturnPhysicalMaterial = true;
    
    // This fires the ray and checks against all objects w/ collision
    FHitResult Hit(ForceInit);
    GetWorld()->LineTraceSingleByObjectType(Hit, start, EndPos, FCollisionObjectQueryParams::AllObjects, TraceParams);
    // Did this hit anything?
    if (Hit.bBlockingHit) {
        AEnemyCharacter* enemy = Cast<AEnemyCharacter>(Hit.GetActor());
        ABullet* projectile = Cast<ABullet>(Hit.GetActor());
        if (enemy) {
            //enemy->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
        } else if (projectile) {
            // do nothing
        } else {
            float dotprod = FVector::DotProduct(Hit.ImpactNormal, -fwd);
            FVector newFwd = 2 * dotprod * Hit.ImpactNormal + fwd;
            count += 1;
            
            newFwd.Normalize();
            
            UGameplayStatics::SpawnEmitterAtLocation(this, BeamFX, Hit.ImpactPoint, newFwd.Rotation());
            WeaponTrace(Hit.ImpactPoint, newFwd, count);
        }
    }
}

void AWeapon::OnStartAim() {
	if (MyOwner != nullptr) {
		if (!MyOwner->IsAlive()) {
			return;
		}
	}
	BeamPSC = PlayBeamEffect(BeamFX);
	GetWorldTimerManager().SetTimer(WeaponTimer, this, &AWeapon::Bounce, FireRate, true);
}

void AWeapon::OnStopAim() {
	if (BeamPSC != nullptr) {
		BeamPSC->DeactivateSystem();
	}
	GetWorldTimerManager().ClearTimer(WeaponTimer);
}

void AWeapon::FireBullet() {
	if (ammo > 0) {
		static FName MuzzleSocket = FName(TEXT("MuzzleFlashSocket"));
		FVector StartPos = WeaponMesh->GetSocketLocation(MuzzleSocket);
		FVector Forward = MyOwner->GetController()->GetActorForwardVector();
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		if (World && bullet && MyOwner->IsAlive()) {
            // spawn bullet
			ABullet* pew = World->SpawnActor<ABullet>(bullet, StartPos, Forward.Rotation(), SpawnParams);
			pew->SetMyWeapon(this);
			ammo -= 1;
            // bullet sound
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletSound, GetActorLocation());
		}
    } else {
        // no ammo sound
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoAmmoSound, GetActorLocation());
    }
}

UParticleSystemComponent* AWeapon::PlayBeamEffect(UParticleSystem* Effects) {
    UParticleSystemComponent* PSC = NULL;
    if (Effects) {
        PSC = UGameplayStatics::SpawnEmitterAttached(Effects, WeaponMesh, TEXT("MuzzleFlashSocket"));
    }
    return PSC;
}