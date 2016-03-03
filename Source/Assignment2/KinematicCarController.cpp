// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicCarController.h"

void AKinematicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		updateTarget();

		if (waypointReached()) {
			velocity = FVector(0, 0, 0);
			// TODO
		} else {
			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			velocity = getVelocity(deltaSec);

			FVector currentLocation = agent->GetActorLocation();

			FVector newLocation = currentLocation + velocity;

			agent->SetActorLocation(newLocation);

			velocity /= deltaSec;
		}
	}
}

void AKinematicCarController::rotate(float deltaSec)
{
	float tarRot = getRotation(agent->GetActorLocation(), target).Yaw;	// Target rotation

	float curRot = agent->GetActorRotation().Yaw;						// Current rotation

	tarRot = tarRot - curRot;

	float clampedRotation = UKismetMathLibrary::ClampAngle(tarRot, -maxAngle, maxAngle);

	clampedRotation *= deltaSec;

	float angle = UKismetMathLibrary::DegTan(clampedRotation);

	angle *= (vMax / L);

	angle = UKismetMathLibrary::RadiansToDegrees(angle);

	angle += curRot;

	agent->SetActorRotation(FRotator(0, angle, 0));

	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Angle: %s"), *FString::SanitizeFloat(angle)));
}

FVector AKinematicCarController::getVelocity(float deltaSec)
{
	rotate(deltaSec);

	FVector newVelocity(0, 0, 0);

	float hyp = deltaSec * vMax;

	newVelocity.X = getXVelocity(hyp);

	newVelocity.Y = getYVelocity(hyp);

	newVelocity.GetClampedToSize(-hyp, hyp);

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
