// Fill out your copyright notice in the Description page of Project Settings.

#include "Ricochet.h"
#include "Weapon.h"
#include "MainCharacter.h"
#include "MainCharacterMovementComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"

AMainCharacter::AMainCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UMainCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) {
 	
	PrimaryActorTick.bCanEverTick = true;
    
    // noise emitter comp
    NoiseEmitterComp = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("NoiseEmitterComp"));
    
    // character movement component
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->MaxWalkSpeedCrouched = 150;
    
    // Enable crouching
    MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
    
    // set up camera boom properties
    CameraBoomComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
    CameraBoomComp->SocketOffset = FVector(0, 50, 0);
    CameraBoomComp->TargetOffset = FVector(0, 0, 55);
    CameraBoomComp->bUsePawnControlRotation = true;
    CameraBoomComp->AttachParent = GetRootComponent();
    
    // initialize camera
    CameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
    CameraComp->AttachParent = CameraBoomComp;
    
    // specify aiming/sprinting speed
    TargetingSpeedModifier = 0.5f;
    SprintingSpeedModifier = 2.5f;
    
    // character properties
    Health = 1;
    
    MyWeapon = nullptr;
}

void AMainCharacter::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
    
    if (!IsSprinting() && bWantsToRun) {
        SetSprinting(true);
    }
}

void AMainCharacter::BeginPlay() {
    // Call base class BeginPlay
    Super::BeginPlay();
    
    // Spawn the weapon, if one was specified
    if (WeaponClass) {
        UWorld* World = GetWorld();
        if (World) {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Instigator;
            
            // Need to set rotation like this because otherwise gun points down
            FRotator Rotation(65.0f, 0.0f, -180.0f);
            
            // Spawn the Weapon
            MyWeapon = World->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, Rotation, SpawnParams);
            
            if (MyWeapon) {
                // This is attached to "WeaponPoint" which is defined in the skeleton
                MyWeapon->AttachRootComponentTo(GetMesh(), TEXT("WeaponPoint"));
                // attach owner
                MyWeapon->SetMyOwner(this);
            }
        }
    }
}

void AMainCharacter::OnStartAim() {
    if (MyWeapon != nullptr) {
        if (this->IsTargeting()) {
            MyWeapon->OnStartAim();
        }
    }
}

void AMainCharacter::OnStopAim() {
    if (MyWeapon != nullptr) {
        MyWeapon->OnStopAim();
    }
}

void AMainCharacter::FireBullet() {
	if (MyWeapon != nullptr) {
		if (this->IsTargeting()) {
			MyWeapon->FireBullet();
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);
    
    // Movement
    InputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
    InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    InputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::OnStartSprinting);
    InputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::OnStopSprinting);
    InputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::OnCrouch);
    
    // Weapons
    InputComponent->BindAction("Targeting", IE_Pressed, this, &AMainCharacter::OnTargeting);
    InputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::FireBullet);
}

void AMainCharacter::MoveForward(float Val) {
    if (Controller && Val != 0.0f) {
        const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
        const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        
        AddMovementInput(Direction, Val);
    }
}


void AMainCharacter::MoveRight(float Val) {
    if (Val != 0.0f) {
        const FRotator Rotation = GetActorRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        AddMovementInput(Direction, Val);
    }
}

void AMainCharacter::OnTargeting() {
    if(IsTargeting()) {
        SetTargeting(false);
        OnStopAim();
    } else {
        SetTargeting(true);
        OnStartAim();
    }
}


void AMainCharacter::SetTargeting(bool NewVal) {
    bIsTargeting = NewVal;
}


bool AMainCharacter::IsTargeting() const {
    return bIsTargeting;
}


float AMainCharacter::GetTargetingSpeedModifier() const {
    return TargetingSpeedModifier;
}

void AMainCharacter::SetSprinting(bool NewVal) {
    bWantsToRun = NewVal;
    
    if (bIsCrouched) {
        UnCrouch();
    }
}


void AMainCharacter::OnStartSprinting() {
    SetSprinting(true);
}


void AMainCharacter::OnStopSprinting() {
    SetSprinting(false);
}


bool AMainCharacter::IsSprinting() const {
    if (!GetCharacterMovement())
        return false;
    
    // don't allow diagonal sprinting
    return bWantsToRun && !IsTargeting() && !GetVelocity().IsZero() && !bIsCrouched
    && ((GetVelocity().GetSafeNormal2D() | GetActorRotation().Vector()) > 0.8);
}


float AMainCharacter::GetSprintingSpeedModifier() const {
    return SprintingSpeedModifier;
}


void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(AMainCharacter, bWantsToRun, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(AMainCharacter, bIsTargeting, COND_SkipOwner);
    DOREPLIFETIME(AMainCharacter, Health);
}

void AMainCharacter::OnCrouch() {
    
    if (CanCrouch()) {
        Crouch();
    } else {
        UnCrouch();
    }
}


FRotator AMainCharacter::GetAimOffsets() const {
    const FVector AimDirWS = GetBaseAimRotation().Vector();
    const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
    const FRotator AimRotLS = AimDirLS.Rotation();
    
    return AimRotLS;
}


float AMainCharacter::GetHealth() const {
    return Health;
}

bool AMainCharacter::IsAlive() const {
    return Health > 0;
}

void AMainCharacter::EmitSound(float Strength) {
    // make noise at specifed strength
    MakeNoise(Strength, this, GetActorLocation());
}

float AMainCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
    
    float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
    
    // decrement health
    Health--;
    
    // we're dead, don't allow further damage
    bCanBeDamaged = false;
    
    // play death animation
    float DeathTime = PlayAnimMontage(DeathAnim);
    
    // play death sound
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
    
    // enter cinematic mode
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC) {
        PC->SetCinematicMode(true, true, true);
    }
    
    // stop player from getting up after death
    GetWorldTimerManager().SetTimer(DeathTimer, this, &AMainCharacter::StayDown, DeathTime - 0.25f, false);
    
    return ActualDamage;
}

void AMainCharacter::StayDown() {
    OnStopAim();
    
    // deactivate mesh
    GetMesh()->Deactivate();
    
    // reload level
    UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
}

