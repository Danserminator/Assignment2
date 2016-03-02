// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicPointController.h"


#define OUTPUT

void AKinematicPointController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		updateTarget();

		if (waypointReached()) {
			// TODO
		}
		else {
			velocity = getVelocity();

			FVector currentLocation = agent->GetActorLocation();

			FVector newLocation = currentLocation + velocity;

			setRotation();

			agent->SetActorLocation(newLocation);
			//agent->SetActorLocationAndRotation(newLocation, rotation);
		}
	}
}

FVector AKinematicPointController::getVelocity() const
{
	FVector newVelocity;

	float rotation = getRotation(agent->GetActorLocation(), target).Yaw;

	newVelocity = vMax * FVector(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation), 0);

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = FMath::Abs(remainingDistance.X);
	remainingDistance.Y = FMath::Abs(remainingDistance.Y);

	float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

	newVelocity *= deltaSec;

	newVelocity.X = FMath::Clamp(newVelocity.X, -remainingDistance.X, remainingDistance.X);
	newVelocity.Y = FMath::Clamp(newVelocity.Y, -remainingDistance.Y, remainingDistance.Y);

	return newVelocity;
}
