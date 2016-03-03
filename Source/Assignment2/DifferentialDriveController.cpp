// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DifferentialDriveController.h"

#define OUTPUT

ADifferentialDriveController::ADifferentialDriveController()
{
	float errorTolerance = 0.001;
}

void ADifferentialDriveController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		updateTarget();

		if (waypointReached()) {
			// TODO
		} else {
			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			if (rotating) {
				rotating = !rotate(deltaSec);
			} else {
				velocity = getVelocity(deltaSec);

				FVector currentLocation = agent->GetActorLocation();

				FVector newLocation = currentLocation + velocity;

				agent->SetActorLocation(newLocation);
			}
		}
	}
}

bool ADifferentialDriveController::rotate(float deltaSec)
{
	float rotation = getRotation(agent->GetActorLocation(), target);

	rotation -= agent->GetActorRotation().Yaw;

	float currentMaxAngle = maxAngle * deltaSec;

	float clampedRotation = UKismetMathLibrary::ClampAngle(rotation, -currentMaxAngle, currentMaxAngle);

	agent->SetActorRotation(FRotator(0, agent->GetActorRotation().Yaw + clampedRotation, 0));

	return abs(rotation - clampedRotation) < 0.0001;
}

FVector ADifferentialDriveController::getVelocity(float deltaSec)
{
	FVector newVelocity(0, 0, 0);

	if (!rotate(deltaSec)) {
		rotating = true;
		return FVector(0, 0, 0);	// Still need to turn
	}

	newVelocity.X = getXVelocity(deltaSec);

	newVelocity.Y = getYVelocity(deltaSec);

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = UKismetMathLibrary::Abs(remainingDistance.X);
	remainingDistance.Y = UKismetMathLibrary::Abs(remainingDistance.Y);

	newVelocity.X = UKismetMathLibrary::FClamp(newVelocity.X, -remainingDistance.X, remainingDistance.X);
	newVelocity.Y = UKismetMathLibrary::FClamp(newVelocity.Y, -remainingDistance.Y, remainingDistance.Y);

	return newVelocity;
}

float ADifferentialDriveController::getXVelocity(float deltaSec) const
{
	return deltaSec * vMax * UKismetMathLibrary::DegCos(agent->GetActorRotation().Yaw);
}

float ADifferentialDriveController::getYVelocity(float deltaSec) const
{
	return deltaSec * vMax * UKismetMathLibrary::DegSin(agent->GetActorRotation().Yaw);
}