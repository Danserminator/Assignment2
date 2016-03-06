// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicCarController.h"

ADynamicCarController::ADynamicCarController()
{
	errorTolerance = 3;
}

//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Position: %s -> %s"), *to2D(agent->GetActorLocation()).ToString(), *target.ToString()));

void ADynamicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		updateTarget();

		if (first) {
			first = false;
			FRotator rotation = agent->GetActorRotation();
			rotation.Yaw = getRotation(agent->GetActorLocation(), target);
			agent->SetActorRotation(rotation);

			waypoints = DubinsPath::getPath(waypoints, to2D(agent->GetActorLocation()), rotation.Yaw, maxAngle, L, graph, errorTolerance);

			writeWaypointsToFile("Waypoints2.txt");

			if (waypoints.Num() > 0) {
				target = waypoints[0];
			}
		}

		if (waypointReached()) {
			return;
		}

		DrawDebugLine(GWorld->GetWorld(), to3D(target), to3D(target) + collisionSize, collisionColor, false, 0.1, 0, 1);

		float a = getAcceleration(deltaSec);

		v += a;

		v = UKismetMathLibrary::FClamp(v, -vMax, vMax);

		float rotation = rotate(deltaSec);

		acceleration.X = a * UKismetMathLibrary::DegCos(rotation);
		acceleration.Y = a * UKismetMathLibrary::DegSin(rotation);

		drawLine(2 * acceleration / deltaSec, accelerationColor);

		velocity = FVector(v * UKismetMathLibrary::DegCos(rotation) * deltaSec, v * UKismetMathLibrary::DegSin(rotation) * deltaSec, 0);

		float rot = agent->GetActorRotation().Yaw;

		setRotation();

		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("%f"), UKismetMathLibrary::Abs(rot - agent->GetActorRotation().Yaw) / deltaSec));

		agent->SetActorLocation(agent->GetActorLocation() + velocity);

		DrawDebugLine(GWorld->GetWorld(), agent->GetActorLocation(), agent->GetActorLocation() + collisionSize, FColor::Green, false, 0.1, 0, 1);
	}
}

bool ADynamicCarController::lookingAtTarget()
{
	float wantedRotation = getRotation(agent->GetActorLocation(), target);

	float realRotation = agent->GetActorRotation().Yaw;

	return UKismetMathLibrary::Abs(wantedRotation - realRotation) < 0.1;
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
	float rotation = positiveAngle(AModelController::getRotation(agent->GetActorLocation(), target));

	rotation -= agent->GetActorRotation().Yaw;

	float curMaxAngle = maxAngle * deltaSec;					// Max angle for this tick

	rotation = UKismetMathLibrary::ClampAngle(positiveAngle(rotation), -curMaxAngle, curMaxAngle);

	rotation = UKismetMathLibrary::DegTan(positiveAngle(rotation));

	rotation *= v / L;

	rotation += UKismetMathLibrary::DegreesToRadians(agent->GetActorRotation().Yaw);

	return UKismetMathLibrary::RadiansToDegrees(rotation);
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