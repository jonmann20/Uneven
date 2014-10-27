// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Uneven.h"
#include "UnevenGameMode.h"
#include "UnevenPawn.h"

AUnevenGameMode::AUnevenGameMode(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// set default pawn class to our character class
	DefaultPawnClass = AUnevenPawn::StaticClass();
}

