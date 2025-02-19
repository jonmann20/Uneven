// Copyright 1998-2014 Epic Games, Inc. All Rights Reserve

#include "Uneven.h"
#include "UnevenProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AUnevenProjectile::AUnevenProjectile(const class FPostConstructInitializeProperties& PCIP) 
	: Super(PCIP)
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Meshes/SM_Projectile.SM_Projectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->AttachTo(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("PhysicsActor");			// Collision profiles are defined in DefaultEngine.ini
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AUnevenProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 4000.f;
	ProjectileMovement->MaxSpeed = 4000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	// Die after 3 seconds by default
	InitialLifeSpan = 2.5f;
}

void AUnevenProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	// Only add impulse and destroy projectile if we hit a physics
	if((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics()) {
		FDamageEvent f;
		AController* a = nullptr;
		//OtherActor->TakeDamage(1, f, a, this);
		OtherActor->Destroy();
	}
}