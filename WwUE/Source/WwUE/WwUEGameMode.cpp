// Copyright Epic Games, Inc. All Rights Reserved.

#include "WwUEGameMode.h"
#include "WwUECharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "WwUECharacter.h"

AWwUEGameMode::AWwUEGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Character/BP_AWwUECharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
