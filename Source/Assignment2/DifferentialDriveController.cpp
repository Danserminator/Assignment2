// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DifferentialDriveController.h"

#define OUTPUT

ADifferentialDriveController::ADifferentialDriveController()
{
}

void ADifferentialDriveController::BeginPlay()
{
	agent = static_cast<AAgent *>(GetPawn());	// Check if can be set in constructor.

	R = agent->R;
	formation = agent->formation;
	unseenAgents = agent->unseenAgents;
}

void ADifferentialDriveController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		setTarget();

		if (waypointReached()) {
			// TODO
		}
		else {
			if (rotating) {
				rotating = !rotate();
			} else {
				velocity = getVelocity();

				FVector currentLocation = agent->GetActorLocation();

				FVector newLocation = currentLocation + velocity;

				agent->SetActorLocation(newLocation);
			}
		}
	}
}

bool ADifferentialDriveController::rotate()
{
	float rotation = getRotation(agent->GetActorLocation(), target).Yaw;

	float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

	float currentMaxAngle = maxAngle * deltaSec;

	float clampedRotation = UKismetMathLibrary::ClampAngle(rotation, agent->GetActorRotation().Yaw - currentMaxAngle, agent->GetActorRotation().Yaw + currentMaxAngle);

	agent->SetActorRotation(FRotator(0, clampedRotation, 0));

	if (abs(rotation - clampedRotation) < 0.0001) {
		return true;
	}

	return false;
}

FVector ADifferentialDriveController::getVelocity()
{
	FVector newVelocity(0, 0, 0);

	rotate();

	float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

	float hyp = deltaSec * vMax;

	newVelocity.X = getXVelocity(hyp);

	newVelocity.Y = getYVelocity(hyp);	

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = FMath::Abs(remainingDistance.X);
	remainingDistance.Y = FMath::Abs(remainingDistance.Y);

	//float maxSize = newVelocity.Size() * deltaSec;

	//newVelocity.X = 

	//newVelocity = newVelocity.ClampMaxSize(maxSize);

	newVelocity.X = FMath::Clamp(newVelocity.X, -remainingDistance.X, remainingDistance.X);
	newVelocity.Y = FMath::Clamp(newVelocity.Y, -remainingDistance.Y, remainingDistance.Y);

	return newVelocity;
}

float ADifferentialDriveController::getXVelocity(float hyp) const
{
	return hyp * UKismetMathLibrary::DegCos(agent->GetActorRotation().Yaw);
}

float ADifferentialDriveController::getYVelocity(float hyp) const
{
	return hyp * UKismetMathLibrary::DegSin(agent->GetActorRotation().Yaw);
}