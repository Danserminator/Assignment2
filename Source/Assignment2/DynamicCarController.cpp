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
			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			acceleration = getAcceleration(deltaSec);

			drawLine(2 * acceleration, accelerationColor);

			velocity += acceleration;

			velocity = velocity.GetClampedToSize(-vMax, vMax);

			FVector FrameVelocity = getVelocity(deltaSec);

			agent->SetActorLocation(agent->GetActorLocation() + FrameVelocity);
		}
	}
}

FVector ADynamicCarController::getAcceleration(float deltaSec)
{
	float angle = rotate(deltaSec);

	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Angle: %s"), *FString::SanitizeFloat(angle)));

	FVector newAcceleration(0, 0, 0);

	newAcceleration.X = getXAcceleration(angle, deltaSec);
	newAcceleration.Y = getYAcceleration(angle, deltaSec);

	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Angle: %s"), *newAcceleration.ToString()));

	float velocityLength = to2D(velocity).Size(); // UKismetMathLibrary::VSize(velocity);

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

float ADynamicCarController::rotate(float deltaSec)
{
	float rotation = AModelController::getRotation(agent->GetActorLocation(), target).Yaw;

	rotation -= agent->GetActorRotation().Yaw;

	float curMaxAngle = deltaSec * maxAngle;					// Max angle for this tick

	rotation = UKismetMathLibrary::ClampAngle(rotation, -curMaxAngle, curMaxAngle);

	rotation = UKismetMathLibrary::DegTan(rotation);

	rotation *= (aMax / L);

	rotation = UKismetMathLibrary::RadiansToDegrees(rotation);

	rotation += agent->GetActorRotation().Yaw;

	//agent->SetActorRotation(FRotator(0, rotation, 0));

	return rotation;
}

float ADynamicCarController::getXAcceleration(float angle, float deltaSec)
{
	return deltaSec * aMax * UKismetMathLibrary::DegCos(angle);
}

float ADynamicCarController::getYAcceleration(float angle, float deltaSec)
{
	return deltaSec * aMax * UKismetMathLibrary::DegSin(angle);
}

FVector ADynamicCarController::getVelocity(float deltaSec)
{
	rotateVelocity(deltaSec);

	FVector newVelocity(0, 0, 0);

	newVelocity.X = getXVelocity(deltaSec);
	newVelocity.Y = getYVelocity(deltaSec);

	return newVelocity;
}

float ADynamicCarController::rotateVelocity(float deltaSec)
{
	float rotation = AModelController::getRotation(agent->GetActorLocation(), target).Yaw;

	rotation -= agent->GetActorRotation().Yaw;

	float curMaxAngle = deltaSec * maxAngle;					// Max angle for this tick

	rotation = UKismetMathLibrary::ClampAngle(rotation, -curMaxAngle, curMaxAngle);

	rotation = UKismetMathLibrary::DegTan(rotation);

	rotation *= (to2D(velocity).Size() / L);

	rotation = UKismetMathLibrary::RadiansToDegrees(rotation);

	rotation += agent->GetActorRotation().Yaw;

	agent->SetActorRotation(FRotator(0, rotation, 0));

	return rotation;
}

float ADynamicCarController::getXVelocity(float deltaSec)
{
	return deltaSec * to2D(velocity).Size() * UKismetMathLibrary::DegCos(agent->GetActorRotation().Yaw);
}

float ADynamicCarController::getYVelocity(float deltaSec)
{
	return deltaSec * to2D(velocity).Size() * UKismetMathLibrary::DegSin(agent->GetActorRotation().Yaw);
}
