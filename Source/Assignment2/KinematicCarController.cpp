// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicCarController.h"

void AKinematicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		setTarget();

		if (waypointReached()) {
			// TODO
		} else {
			velocity = getVelocity();

			FVector currentLocation = agent->GetActorLocation();

			FVector newLocation = currentLocation + velocity;

			agent->SetActorLocation(newLocation);
		}
	}
}

void AKinematicCarController::rotate()
{
	float tarRot = getRotation(agent->GetActorLocation(), target).Yaw;	// Target rotation

	float curRot = agent->GetActorRotation().Yaw;						// Current rotation

	tarRot = tarRot - curRot;

	float clampedRotation = UKismetMathLibrary::ClampAngle(tarRot, -maxAngle, maxAngle);

	float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

	clampedRotation *= deltaSec;

	clampedRotation = FMath::DegreesToRadians(clampedRotation);

	float angle = UKismetMathLibrary::Tan(clampedRotation);

	angle *= (vMax / L);

	angle = FMath::RadiansToDegrees(angle) + curRot;

	agent->SetActorRotation(FRotator(0, angle, 0));

	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Angle: %s"), *FString::SanitizeFloat(angle)));
}

FVector AKinematicCarController::getVelocity()
{
	rotate();

	FVector newVelocity(0, 0, 0);

	float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

	float hyp = deltaSec * vMax;

	newVelocity.X = getXVelocity(hyp);

	newVelocity.Y = getYVelocity(hyp);

	newVelocity.GetClampedToSize(-hyp, hyp);				// TODO: GetClampMaxSize?

	return newVelocity;
}

float AKinematicCarController::getXVelocity(float hyp) const
{
	return hyp * UKismetMathLibrary::DegCos(agent->GetActorRotation().Yaw);
}

float AKinematicCarController::getYVelocity(float hyp) const
{
	return hyp * UKismetMathLibrary::DegSin(agent->GetActorRotation().Yaw);
}
