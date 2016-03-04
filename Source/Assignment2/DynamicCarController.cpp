// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicCarController.h"

ADynamicCarController::ADynamicCarController()
{
	errorTolerance = 10;
}

//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Position: %s -> %s"), *to2D(agent->GetActorLocation()).ToString(), *target.ToString()));

void ADynamicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		if (!movingFormation) {
			updateTarget();

			if (waypointReached()) {
				// TODO
			} else {
				if (first) {
					first = false;
					FRotator rotation = agent->GetActorRotation();
					rotation.Yaw = getRotation(agent->GetActorLocation(), target);
					agent->SetActorRotation(rotation);
				}

				if (lookingAtTarget() || true) {
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
				} else {

				}
			}
		} else {
			if (moveTarget || agent->numberUnseenAgents() > 0) {
				updateTarget();

				if (first) {
					first = false;
					FRotator rotation = agent->GetActorRotation();
					rotation.Yaw = getRotation(agent->GetActorLocation(), target);
					agent->SetActorRotation(rotation);
				}

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
				} else {
					if (first) {
						first = false;
						FRotator rotation = agent->GetActorRotation();
						rotation.Yaw = getRotation(agent->GetActorLocation(), target);
						agent->SetActorRotation(rotation);
					}

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
	}
}

bool ADynamicCarController::lookingAtTarget()
{
	float wantedRotation = getRotation(agent->GetActorLocation(), target);

	float realRotation = agent->GetActorRotation().Yaw;

	return UKismetMathLibrary::Abs(wantedRotation - realRotation) < 0.1;
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

	float curMaxAngle = maxAngle;					// Max angle for this tick

	rotation = UKismetMathLibrary::ClampAngle(rotation, -maxAngle, maxAngle);

	rotation = deltaSec * UKismetMathLibrary::RadiansToDegrees(UKismetMathLibrary::DegTan(rotation));

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

bool ADynamicCarController::updateTarget_moving()
{
	FVector2D oldTarget = target;

	// The agent is following a moving formation.

	agent->findAgents();	// Check if we can see any new agents.

	if (agent->numberUnseenAgents() > 0) {
		// We cannot see all the other agents so move towards the agents we can see.
		searching = true;
		target = approachAgents();

	}
	else {
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

		}
		catch (std::exception e) {
			// At least one of the other agents do not know where everybody else are.
			// I will move towards all the other agents.
			target = approachAgents();
		}
	}

	return target.Equals(oldTarget, 0.001);
}

void ADynamicCarController::simulate()
{

}