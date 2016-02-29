// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicCarController.h"

AKinematicCarController::AKinematicCarController()
{
}

void AKinematicCarController::BeginPlay()
{
	agent = static_cast<AAgent *>(GetPawn());	// Check if can be set in constructor.

	R = agent->R;
	formation = agent->formation;
	unseenAgents = agent->unseenAgents;
}

void AKinematicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
#ifdef OUTPUT
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Hej från kinematic car")));
#endif

		setTarget();

		if (waypointReached()) {
			// TODO
		}
		else {
			velocity = getVelocity();

			FVector currentLocation = agent->GetActorLocation();

			FVector newLocation = currentLocation + velocity;

			agent->SetActorLocation(newLocation);
		}
	}
}

void AKinematicCarController::rotate()
{
	
}

FVector AKinematicCarController::getVelocity()
{
	return FVector();
}

float AKinematicCarController::getXVelocity() const
{
	return 0.0f;
}

float AKinematicCarController::getYVelocity() const
{
	return 0.0f;
}
