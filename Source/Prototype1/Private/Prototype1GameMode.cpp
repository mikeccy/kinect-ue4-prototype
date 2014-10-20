// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Prototype1.h"
#include "Prototype1GameMode.h"
#include "Prototype1Character.h"

APrototype1GameMode::APrototype1GameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
