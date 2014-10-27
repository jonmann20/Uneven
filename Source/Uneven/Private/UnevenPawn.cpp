// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Uneven.h"
#include "UnevenPawn.h"
#include "UnevenProjectile.h"
#include "TimerManager.h"

const FName AUnevenPawn::ShootBinding("Shoot");

//const FName AUnevenPawn::FireForwardBinding("FireForward");
//const FName AUnevenPawn::FireRightBinding("FireRight");

AUnevenPawn::AUnevenPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{	
	static FName CollisionProfileName(TEXT("Pawn"));	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/Meshes/UFO.UFO"));
	// Create the mesh component
	ShipMeshComponent = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(CollisionProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/Audio/TemplateTSS_WeaponFire.TemplateTSS_WeaponFire"));
	FireSound = FireAudio.Object;

	// Create a spring arm component
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("SpringArm0"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 60.f);
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->CameraLagSpeed = 15.f;

	// Create camera component 
	CameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera0"));
	CameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Movement
	MoveSpeed = 1000.0f;
	// Weapon
	GunOffset = FVector(290.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;

	Acceleration = 1000.f;
	TurnSpeedX = 88.f;
	TurnSpeedY = 56.f;
	MaxSpeed = 4200.f;
	MinSpeed = 420.f;
	CurrentForwardSpeed = 950.f;
}

void AUnevenPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	
	InputComponent->BindAxis("MoveUp", this, &AUnevenPawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &AUnevenPawn::MoveRightInput);
	InputComponent->BindAxis(ShootBinding);

	//InputComponent->BindAxis(FireForwardBinding);
	//InputComponent->BindAxis(FireRightBinding);
}

void AUnevenPawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);
	
	// Create fire direction vector
	//const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	//const float FireRightValue = GetInputAxisValue(FireRightBinding);
	//const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);
	const float shootValue = GetInputAxisValue(ShootBinding);

	UE_LOG(LogTemp, Warning, TEXT("shootVal: %f"), shootValue);

	if (shootValue > 0.f) {
		FireShot(FVector(0, 0, 0));
	}
}

void AUnevenPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeedY);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = TargetPitchSpeed;//FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 1.f);
}

void AUnevenPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeedX);

	TargetYawSpeed = FMath::Clamp(TargetYawSpeed, -90.f, 90.f);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = TargetYawSpeed;//FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);



	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.2f) : (GetActorRotation().Roll * -5.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = TargetRollSpeed;//FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}


void AUnevenPawn::FireShot(FVector FireDirection)
{
	if (bCanFire == true)
	{
		const FRotator FireRotation = GetActorRotation();
		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile
			World->SpawnActor<AUnevenProjectile>(SpawnLocation, FireRotation);
		}

		bCanFire = false;
		World->GetTimerManager().SetTimer(this, &AUnevenPawn::ShotTimerExpired, FireRate);

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void AUnevenPawn::ShotTimerExpired()
{
	bCanFire = true;
}

