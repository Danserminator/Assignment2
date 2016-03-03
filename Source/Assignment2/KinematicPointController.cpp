// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicPointController.h"


#define OUTPUT

AKinematicPointController::AKinematicPointController()
{
	errorTolerance = 0.001;
}

void AKinematicPointController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		updateTarget();

		if (waypointReached()) {
			// TODO
		} else {
			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			velocity = getVelocity(deltaSec);

			FVector currentLocation = agent->GetActorLocation();

			FVector newLocation = currentLocation + velocity;

			setRotation();

			agent->SetActorLocation(newLocation);
			//agent->SetActorLocationAndRotation(newLocation, rotation);
		}
	}
}

FVector AKinematicPointController::getVelocity(float deltaSec) const
{
	FVector newVelocity;

	float rotation = getRotation(agent->GetActorLocation(), target).Yaw;

	newVelocity = deltaSec * vMax * FVector(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation), 0);

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = UKismetMathLibrary::Abs(remainingDistance.X);
	remainingDistance.Y = UKismetMathLibrary::Abs(remainingDistance.Y);

	newVelocity.X = UKismetMathLibrary::FClamp(newVelocity.X, -remainingDistance.X, remainingDistance.X);
	newVelocity.Y = UKismetMathLibrary::FClamp(newVelocity.Y, -remainingDistance.Y, remainingDistance.Y);

	return newVelocity;
}
