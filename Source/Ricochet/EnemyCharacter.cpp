// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "Engine/TargetPoint.h"
#include "Perception/PawnSensingComponent.h"
#include "MainCharacter.h"
#include "EnemyWeapon.h"
#include "MainCharacterMovementComponent.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UMainCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) {
 	
	PrimaryActorTick.bCanEverTick = true;
    
    // set default AI controller
    AIControllerClass = AEnemyAIController::StaticClass();
    
    // character movement component
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->MaxWalkSpeedCrouched = 150;
    
    // Enable crouching
    MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

    // initialize pawn sensing component
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SetPeripheralVisionAngle(60.0f);
    PawnSensingComp->SightRadius = 2000;
    PawnSensingComp->HearingThreshold = 700;
    PawnSensingComp->LOSHearingThreshold = 1200;
    
    // initialize weapon
    MyWeapon = nullptr;
    
    // specify aiming/sprinting speed
    TargetingSpeedModifier = 0.5f;
    SprintingSpeedModifier = 3.5f;

	SetActorEnableCollision(true);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay() {
	Super::BeginPlay();
    
    if (PawnSensingComp) {
        // senses to be detected by AI
        PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSenseVisual);
        PawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyCharacter::OnSenseNoise);
    }
    
    // Spawn the weapon, if one was specified
    if (WeaponClass) {
        UWorld* World = GetWorld();
        if (World) {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Instigator;
            // Need to set rotation like this because otherwise gun points down
            FRotator Rotation(65.0f, 25.0f, -130.0f);
            
            // Spawn the Weapon
            MyWeapon = World->SpawnActor<AEnemyWeapon>(WeaponClass, FVector::ZeroVector, Rotation, SpawnParams);
            
            if (MyWeapon) {
                // This is attached to "WeaponPoint" which is defined in the skeleton
                MyWeapon->AttachRootComponentTo(GetMesh(), TEXT("WeaponPoint"));
                // attach owner
                MyWeapon->SetMyOwner(this);
            }
        }
    }
}

// Called every frame
void AEnemyCharacter::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

    // check whether player is not sensed anymore
    if (PlayerSensed && ((GetWorld()->TimeSeconds - TimeSinceSeen) > 4.0f) && ((GetWorld()->TimeSeconds - TimeSinceHeard) > 4.0f)) {
        AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
        if (AIController) {
            // check whether already attacking
            if(!AIController->GetAttackOnly()) {
                // stop sprinting
                SetSprinting(false);
                
                // set player as not sensed
                PlayerSensed = false;
                
                // restart state machine
                AIController->SetState(Start);
            }
        }
    }
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);

}

void AEnemyCharacter::OnStartFire() {
    // nullptr check weapon
    if (MyWeapon) {
        // check whether dead
        if(!IsDead()) {
            // call weapon OnStartFire()
            MyWeapon->OnStartFire();
        }
    }
}

void AEnemyCharacter::OnStopFire() {
    // nullptr check weapon
    if (MyWeapon) {
        // check whether dead
        if(!IsDead()) {
            // call weapon OnStopFire()
            MyWeapon->OnStopFire();
        }
    }
}

float AEnemyCharacter::TakeDamage(float Damage, const struct FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) {
    // call super function
    float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
    
    // we're dead, don't allow further damage
    bCanBeDamaged = false;
    
    // AI controller
    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    if (AIController) {
        // stop attacking player
        if(AIController->GetAttackOnly()) {
            OnStopFire();
        }
        
        // play death animation
        float DeathTime = PlayAnimMontage(DeathAnim);
        
        // play death sound
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
        
        // unpossess character
        GetController()->UnPossess();
        
        // destroy dwarf after 0.25 sec
        GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemyCharacter::DestroyEnemy, DeathTime - 0.25f, false);
    }
    
    return ActualDamage;
}

void AEnemyCharacter::DestroyEnemy() {
    // destroy enemy actor
    Destroy();
    
    // clear death timer
    GetWorldTimerManager().ClearTimer(DeathTimer);
}

void AEnemyCharacter::OnSenseVisual(APawn* Pawn) {
    AMainCharacter* Player = Cast<AMainCharacter>(Pawn);
    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    
    // check whether already attacking
    if(!AIController->GetAttackOnly()) {
        if (AIController && Player->IsAlive()) {
            if (!PlayerSensed) {
                // alert sound
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), AlertSound, GetActorLocation());
            }
            
            // set player as sensed
            PlayerSensed = true;
            
            // initialize time from player being heard
            TimeSinceHeard = GetWorld()->GetTimeSeconds();
            
            // sprint towards player if heard
            SetSprinting(true);
            GetCharacterMovement()->MaxWalkSpeed = 600.0f;
            AIController->MoveToActor(Player);
            
            // update state machine
            AIController->SetState(Chase);
            
            // clear patrol timer
            GetWorldTimerManager().ClearTimer(AIController->GetPatrolTimer());
        }
    }
}

void AEnemyCharacter::OnSenseNoise(APawn* PawnInstigator, const FVector& Location, float Volume) {
    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    
    // check whether already attacking
    if(!AIController->GetAttackOnly()) {
        if (AIController) {
            if (!PlayerSensed) {
                // alert sound
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), AlertSound, GetActorLocation());
            }
            
            // set player as sensed
            PlayerSensed = true;
            
            // initialize time from player being heard
            TimeSinceHeard = GetWorld()->GetTimeSeconds();
            
            // sprint towards player if heard
            SetSprinting(true);
            GetCharacterMovement()->MaxWalkSpeed = 600.0f;
            AIController->MoveToActor(PawnInstigator);
            
            // update state machine
            AIController->SetState(Chase);
            
            // clear patrol timer
            GetWorldTimerManager().ClearTimer(AIController->GetPatrolTimer());
        }
    }
}

bool AEnemyCharacter::IsDead() {
    // check whether dead or not
    return bIsDead;
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(AEnemyCharacter, bIsTargeting, COND_SkipOwner);
}

void AEnemyCharacter::SetTargeting(bool NewTargeting) {
    bIsTargeting = NewTargeting;
}


bool AEnemyCharacter::IsTargeting() const {
    return bIsTargeting;
}

void AEnemyCharacter::OnCrouch() {
    if (CanCrouch()) {
        Crouch();
    } else {
        UnCrouch();
    }
}

void AEnemyCharacter::SetSprinting(bool NewSprinting) {
    bWantsToRun = NewSprinting;
    
    // stop crouching
    if (bIsCrouched) {
        UnCrouch();
    }
    
    // stop targeting
    if (IsTargeting()) {
        SetTargeting(false);
    }
}

bool AEnemyCharacter::IsSprinting() const {
    bool sprinting;
    
    if (!GetCharacterMovement()) {
        sprinting = false;
    } else {
        sprinting = bWantsToRun;
    }
    
    return sprinting;
}

void AEnemyCharacter::ChasePlayer() {
    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    
    if (AIController) {
        // update state machine
        AIController->SetState(Chase);
        
        // initialize time from player being heard
        TimeSinceHeard = GetWorld()->GetTimeSeconds();
        
        // sprint towards player if heard
        SetSprinting(true);
        GetCharacterMovement()->MaxWalkSpeed = 600.0f;
        AIController->MoveToActor(PlayerPawn);
    }
}