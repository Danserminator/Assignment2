// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicPointMassController.h"

//#define OUTPUT

ADynamicPointMassController::ADynamicPointMassController()
{
	errorTolerance = 0.01;	// Because this is the bomb!
	vMax = dpmcVMax;
}

// Called every frame
void ADynamicPointMassController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (play) {
		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		if (avoidAgents) {
			updateTarget();

			if (waypointReached()) {
				// TODO
			} else {
				acceleration = getAcceleration(deltaSec);
				FVector vPref = velocity + acceleration;
				vPref = vPref.GetClampedToSize2D(-vMax, vMax);

				FVector oldVel = velocity;

				adjustVelocity(to2D(vPref), deltaSec);

				if (UKismetMathLibrary::Abs(velocity.Size2D() - oldVel.Size2D()) / deltaSec > aMax + 0.1) {
					GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("Acceleration: %f\r\n"), UKismetMathLibrary::Abs(velocity.Size2D() - oldVel.Size2D()) / deltaSec));
				}

				drawLine(2 * (velocity - oldVel) / deltaSec, accelerationColor);

				//FVector2D q = to2D(velocity - oldVel);
				//float dv = FMath::Sqrt(FVector2D::DotProduct(q,q));
				//float dv = velocity.Size() - oldVel.Size();

				//if (dv < aMax * deltaSec) {
				//} else {
				//	float f = aMax * deltaSec / dv;
				//	velocity = (1 - f) * oldVel + f * velocity;
				//}

				/*
				acceleration = getAcceleration(deltaSec);

				drawLine(2 * acceleration, accelerationColor);

				FVector vPref = velocity + acceleration;
				vPref = vPref.GetClampedToSize(-vMax, vMax);

				adjustVelocity(to2D(vPref), deltaSec);

				if (vPref.Equals(velocity)) {
					GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("chose vPref\r\n")));
				} else {
					GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("did not chose vPref\r\n")));
				}
				*/

				FVector currentLocation = agent->GetActorLocation();

				FVector newLocation = currentLocation + (velocity * deltaSec);

				setRotation();

				agent->SetActorLocation(newLocation);
			}

		} else {
			updateTarget();

			if (waypointReached()) {
				// TODO
			}
			else {

				acceleration = getAcceleration(deltaSec);

				//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("Acceleration: %f\r\n"), acceleration.Size2D() / deltaSec));

				drawLine(2 * acceleration, accelerationColor);

				velocity += acceleration;

				if (!everybodyKnows && !movingFormation) {
					velocity = velocity.GetClampedToSize(-everybodyKnowsSpeed, everybodyKnowsSpeed);
				}
				else {
					velocity = velocity.GetClampedToSize(-vMax, vMax);
				}

				FVector currentLocation = agent->GetActorLocation();

				FVector newLocation = currentLocation + (velocity * deltaSec);

				setRotation();

				agent->SetActorLocation(newLocation);
				//agent->SetActorLocationAndRotation(newLocation, rotation);
			}
		} /*else {
			//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("%s -> %s"), *to2D(agent->GetActorLocation()).ToString(), *target.ToString()));
			if (moveTarget || agent->numberUnseenAgents() > 0) {
				updateTarget();

				acceleration = getAcceleration(deltaSec);

				drawLine(2 * acceleration, accelerationColor);

				velocity += acceleration;
				
				velocity = velocity.GetClampedToSize(-vMax, vMax);

				FVector currentLocation = agent->GetActorLocation();

				FVector newLocation = currentLocation + (velocity * deltaSec);

				setRotation();

				agent->SetActorLocation(newLocation);
			} else {
				if (firstTry) {
					firstTry = false;
					updateTarget();
					return;
				} else if (secondTry) {
					secondTry = false;
					updateTarget();
				}

				if (waypointReached()) {
					moveTarget = true;
				}
				else {
					acceleration = getAcceleration(deltaSec);

					drawLine(2 * acceleration, accelerationColor);

					acceleration *= deltaSec;

					velocity += acceleration;

					velocity = velocity.GetClampedToSize(-vMax, vMax);

					FVector currentLocation = agent->GetActorLocation();

					FVector newLocation = currentLocation + (velocity * deltaSec);

					setRotation();

					agent->SetActorLocation(newLocation);
				}
			}
		}*/
	}
}

bool ADynamicPointMassController::waypointReached()
{
	if (AModelController::waypointReached()) {
		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		FVector2D frameAcceleration = to2D(getAcceleration(deltaSec));

		FVector2D frameVelocity = to2D(velocity) * deltaSec;

		if (UKismetMathLibrary::Abs(frameVelocity.X) > UKismetMathLibrary::Abs(frameAcceleration.X) || UKismetMathLibrary::Abs(frameVelocity.Y) > UKismetMathLibrary::Abs(frameAcceleration.Y)) {
			// Too high velocity for us to stop in this time frame.
			return false;
		} else {
			// Can stop in this time frame.
			velocity = FVector(0, 0, 0);
			return true;
		}
	}
	
	return false;
}

FVector ADynamicPointMassController::getAcceleration(float deltaSec) const
{
	FVector newAcceleration;

	float rotation = getRotation(agent->GetActorLocation(), target);

	float distLeftLength = (target - to2D(agent->GetActorLocation())).Size() - safetyBuffer;

	FVector wantToGo = distLeftLength * FVector(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation), 0);

	FVector haveToGo = wantToGo - (velocity * (to2D(velocity).Size() / aMax));

	newAcceleration = haveToGo.GetClampedToSize2D(-aMax * deltaSec, aMax * deltaSec);

	return newAcceleration;

	/*
	if (haveToGo.Size() < aMax) {
		haveToGo.max
	}

	newAcceleration = 

	newAcceleration = deltaSec * FVector(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation), 0);

	newAcceleration = aMax * newAcceleration / mass;

	float velocityLength = getBrakeDistance();

	
	//float distLeftLength = UKismetMathLibrary::VSize(to3D(target) - to3D(to2D(agent->GetActorLocation()))) - safetyBuffer;

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
	*/

	/*
	float rotation = getRotation(agent->GetActorLocation(), target);

	newAcceleration = deltaSec * aMax * FVector(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation), 0);

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = UKismetMathLibrary::Abs(remainingDistance.X);
	remainingDistance.Y = UKismetMathLibrary::Abs(remainingDistance.Y);

	newAcceleration.X = UKismetMathLibrary::FClamp(newAcceleration.X, -remainingDistance.X, remainingDistance.X);
	newAcceleration.Y = UKismetMathLibrary::FClamp(newAcceleration.Y, -remainingDistance.Y, remainingDistance.Y);
	*/
	return newAcceleration;
}

float ADynamicPointMassController::getBrakeDistance() const
{
	float velocityLength = velocity.Size2D();

	velocityLength = velocityLength * velocityLength / (aMax * 2);

	return velocityLength;
}

FVector2D ADynamicPointMassController::getBrakeTarget()
{
	FVector2D normVelocity = to2D(velocity);
	normVelocity.Normalize();

	if (to2D(velocity) < normVelocity) {
		normVelocity = to2D(velocity);
	}

	return (to2D(agent->GetActorLocation()) + normVelocity + (errorTolerance * 0.9));
}

bool ADynamicPointMassController::updateTarget_moving()
{
	FVector2D oldTarget = target;

	// The agent is following a moving formation.

	agent->findAgents();	// Check if we can see any new agents.

	if (agent->numberUnseenAgents() > 0) {
		// We cannot see all the other agents so move towards the agents we can see.
		searching = true;
		target = approachAgents();

	} else {
		// We know where all the other agents are.

		formation->foundAllAgents(agent);	// Tell formation that we have found all agents.

		try {
			target = formation->getTarget(agent);

			// Check if I am moving towards the target right now, else move to old target.
			//if (isMovingTowardsTarget(newTarget)) {
				// Not moving towards the target == we should be stopping.

				//target = newTarget;
				//target = to2D(agent->GetActorLocation());	// Move towards myself.
			//}

		} catch (std::exception e) {
			// At least one of the other agents do not know where everybody else are.
			// I will move towards all the other agents.
			target = approachAgents();
		}
	}

	return target.Equals(oldTarget, 0.001);
}

float ADynamicPointMassController::getSearchDistance()
{
	return vMax * vMax / (aMax * 2);
	return FMath::Max(Super::getSearchDistance(), getBrakeDistance() * searchRadiusScalar);
}

FVector2D ADynamicPointMassController::vSample(float deltaSec) {
	FVector2D aCand;
	do {
		aCand = FVector2D(2.0f*rand() - RAND_MAX, 2.0f*rand() - RAND_MAX);
	} while (FVector2D::DotProduct(aCand, aCand) > (((float)RAND_MAX) * ((float)RAND_MAX)));

	aCand *= (aMax / RAND_MAX);

	FVector temp = to3D(aCand).ClampSize2D(-aMax * deltaSec, aMax * deltaSec);

	FVector newVelocity = (velocity + temp).GetClampedToSize2D(-vMax, vMax);

	return to2D(newVelocity);
}
