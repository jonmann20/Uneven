// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/Character.h"
#include "UnevenPawn.generated.h"

UCLASS(Blueprintable)
class AUnevenPawn : public APawn {
	GENERATED_UCLASS_BODY()

	// The mesh component
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly)
	TSubobjectPtr<class UStaticMeshComponent> ShipMeshComponent;
	
	// The camera
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
	TSubobjectPtr<class UCameraComponent> CameraComponent;

	// Camera boom positioning the camera above the character
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
	TSubobjectPtr<class USpringArmComponent> CameraBoom;
	
	// How fast the weapon will fire
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	// Sound to play each time we fire
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	// Fire a shot in the specified direction
	void FireShot(FVector FireDirection);

	// Handler for the fire timer expiry
	void ShotTimerExpired();

	// Static names for axis bindings
	static const FName ShootBinding;

	//static const FName FireForwardBinding;
	//static const FName FireRightBinding;

protected:
	void ThrustInput(float);
	void MoveUpInput(float);
	void MoveRightInput(float);

private:

	// How quickly forward speed changes
	UPROPERTY(Category = Plane, EditAnywhere)
	float Acceleration;

	// How quickly pawn can steer
	UPROPERTY(Category = Plane, EditAnywhere)
	float TurnSpeedX;

	UPROPERTY(Category = Plane, EditAnywhere)
	float TurnSpeedY;

	// Max forward speed
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	// Min forward speed
	UPROPERTY(Category = Yaw, EditAnywhere)
	float MinSpeed;

	// Current forward speed
	float CurrentForwardSpeed;

	// Current yaw speed
	float CurrentYawSpeed;

	// Current pitch speed
	float CurrentPitchSpeed;

	// Current roll speed
	float CurrentRollSpeed;

	// Flag to control firing
	uint32 bCanFire : 1;
};

