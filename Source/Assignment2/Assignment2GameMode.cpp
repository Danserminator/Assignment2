// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Assignment2.h"
#include "Assignment2GameMode.h"
#include "Assignment2PlayerController.h"
#include "Assignment2Character.h"

AAssignment2GameMode::AAssignment2GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AAssignment2PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}