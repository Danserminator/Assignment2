// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicCarController.h"

void ADynamicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		updateTarget();

		if (waypointReached()) {
			// TODO
		} else {
			acceleration = getAcceleration();

			drawLine(5 * acceleration, accelerationColor);
			
			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			acceleration *= deltaSec;

			velocity += acceleration;

			velocity = velocity.GetClampedToSize(-vMax, vMax);

			FVector velocity2 = deltaSec * getVelocity();

			agent->SetActorLocation(agent->GetActorLocation() + velocity2);
		}
	}
}

FVector ADynamicCarController::getAcceleration()
{
	float angle = getRotation();

	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Angle: %s"), *FString::SanitizeFloat(angle)));

	FVector newAcceleration(0, 0, 0);

	newAcceleration.X = getXAcceleration(angle);
	newAcceleration.Y = getYAcceleration(angle);

	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Angle: %s"), *newAcceleration.ToString()));

	float velocityLength = UKismetMathLibrary::VSize(velocity);

	velocityLength = velocityLength * velocityLength / (aMax * 2);

	float distLeftLength = (target - to2D(agent->GetActorLocation())).Size() - safetyBuffer;

	if (velocityLength >= distLeftLength) {
		// Check if we should start breaking
		FVector2D normVelocity = to2D(velocity);
		normVelocity.Normalize();

		FVector2D normDistLeft = target - to2D(agent->GetActorLocation());
		normDistLeft.Normalize();

		if (normVelocity.Equals(normDistLeft, 0.1)) {
			return -newAcceleration;
		}
	}

	return newAcceleration;
}

float ADynamicCarController::getRotation()
{
	float rotation = AModelController::getRotation(agent->GetActorLocation(), target).Yaw;

	float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

	float curMaxAngle = deltaSec * maxAngle;					// Max angle for this tick

	float curRot = agent->GetActorRotation().Yaw;				// Current rotation

	float clampedAngle = UKismetMathLibrary::ClampAngle(rotation, curRot - curMaxAngle, curRot + curMaxAngle);

	clampedAngle = UKismetMathLibrary::DegTan(clampedAngle - curRot);

	clampedAngle *= (aMax / L);

	return curRot + clampedAngle;
}

float ADynamicCarController::getXAcceleration(float angle)
{
	return aMax * UKismetMathLibrary::DegCos(angle);
}

float ADynamicCarController::getYAcceleration(float angle)
{
	return aMax * UKismetMathLibrary::DegSin(angle);
}

FVector ADynamicCarController::getVelocity()
{
	rotate();

	FVector newVelocity(0, 0, 0);

	newVelocity.X = getXVelocity();
	newVelocity.Y = getYVelocity();

	return newVelocity;
}

void ADynamicCarController::rotate()
{
	float rotation = AModelController::getRotation(agent->GetActorLocation(), target).Yaw;

	float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

	float curMaxAngle = deltaSec * maxAngle;					// Max angle for this tick

	float curRot = agent->GetActorRotation().Yaw;				// Current rotation

	float clampedAngle = UKismetMathLibrary::ClampAngle(rotation, curRot - curMaxAngle, curRot + curMaxAngle);

	clampedAngle = UKismetMathLibrary::DegTan(clampedAngle - curRot);

	clampedAngle *= (velocity.Size() / L);

	agent->SetActorRotation(FRotator(0, clampedAngle, 0));
}

float ADynamicCarController::getXVelocity()
{
	return velocity.Size() * UKismetMathLibrary::DegCos(agent->GetActorRotation().Yaw);
}

float ADynamicCarController::getYVelocity()
{
	return velocity.Size() * UKismetMathLibrary::DegSin(agent->GetActorRotation().Yaw);
}
