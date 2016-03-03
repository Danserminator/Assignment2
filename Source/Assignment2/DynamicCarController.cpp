// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicCarController.h"

ADynamicCarController::ADynamicCarController()
{
	errorTolerance = 0.1;
}

void ADynamicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Position: %s -> %s"), *to2D(agent->GetActorLocation()).ToString(), *target.ToString()));
		if (updateTarget()) {
			// For T1 & T2
			if (first) {
				first = false;
				FRotator rotation = agent->GetActorRotation();
				rotation.Yaw = getRotation(agent->GetActorLocation(), target);
				agent->SetActorRotation(rotation);
			}
		}

		if (waypointReached()) {
			// TODO
		} else {
			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			float a = getAcceleration(deltaSec);

			v += a;

			v = UKismetMathLibrary::FClamp(v, -vMax, vMax);

			float rotation = rotate(deltaSec);

			acceleration.X = a * UKismetMathLibrary::DegCos(rotation);
			acceleration.Y = a * UKismetMathLibrary::DegSin(rotation);

			drawLine(2 * acceleration / deltaSec, accelerationColor);

			velocity.X = v * UKismetMathLibrary::DegCos(rotation);
			velocity.Y = v * UKismetMathLibrary::DegSin(rotation);

			setRotation();

			agent->SetActorLocation(agent->GetActorLocation() + (velocity * deltaSec));
		}
	}
}

bool ADynamicCarController::waypointReached()
{
	if (AModelController::waypointReached()) {
		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		float frameAcceleration = getAcceleration(deltaSec);

		float frameVelocity = v * deltaSec;

		if (UKismetMathLibrary::Abs(frameVelocity) > UKismetMathLibrary::Abs(frameAcceleration)) {
			// Too high velocity for us to stop in this time frame.
			return false;
		}
		else {
			// Can stop in this time frame.
			velocity = FVector(0, 0, 0);
			v = 0;
			return true;
		}
	}

	return false;
}

float ADynamicCarController::getAcceleration(float deltaSec) const
{
	float a = aMax * deltaSec;

	float velocityLength = getBrakeDistance();

	float distLeftLength = (target - to2D(agent->GetActorLocation())).Size() - safetyBuffer;

	if (velocityLength >= distLeftLength) {
		// Check if we should start breaking
		FVector2D normVelocity = to2D(velocity);
		normVelocity.Normalize();

		FVector2D normDistLeft = target - to2D(agent->GetActorLocation());
		normDistLeft.Normalize();

		if (normVelocity.Equals(normDistLeft, 0.1)) {
			return -a;
		}
	}

	return a;
}

float ADynamicCarController::rotate(float deltaSec) const
{
	float rotation = AModelController::getRotation(agent->GetActorLocation(), target);

	rotation -= agent->GetActorRotation().Yaw;

	float curMaxAngle = deltaSec * maxAngle;					// Max angle for this tick

	rotation = UKismetMathLibrary::ClampAngle(rotation, -maxAngle, maxAngle);

	rotation = UKismetMathLibrary::DegTan(rotation);

	rotation *= v / L;

	rotation += agent->GetActorRotation().Yaw;

	return rotation;
}

float ADynamicCarController::getBrakeDistance() const
{
	float velocityLength = to2D(velocity).Size(); // UKismetMathLibrary::VSize(velocity);

	velocityLength = velocityLength * velocityLength / (aMax * 2);

	return velocityLength;
}

float ADynamicCarController::getSearchDistance()
{
	return FMath::Max(Super::getSearchDistance(), getBrakeDistance() * searchRadiusScalar);
}
