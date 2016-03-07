// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicCarController.h"

ADynamicCarController::ADynamicCarController()
{
	errorTolerance = 3;
	vMax = dccVMax;
	aMax = dccAMax;
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

			if (followPath) {
				waypoints = DubinsPath::getPath(waypoints, to2D(agent->GetActorLocation()), rotation.Yaw, maxAngle, L, graph, errorTolerance);

				writeWaypointsToFile("Waypoints2.txt");

				if (waypoints.Num() > 0) {
					target = waypoints[0];
				}
			}
		}

		if (waypointReached()) {
			bool t35 = followPath && waypointsIndex >= waypoints.Num();
			bool t4 = avoidAgents && !followPath;

			if (t35 || t4) {
				play = false;
				GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Time: %f\r\n"), totalTime));
			}

			return;
		}

		float a = getAcceleration(deltaSec);

		v += a;

		v = UKismetMathLibrary::FClamp(v, -vMax, vMax);

		float rotation = rotate(deltaSec);

		FVector vPref = FVector(v * UKismetMathLibrary::DegCos(rotation) * deltaSec, v * UKismetMathLibrary::DegSin(rotation) * deltaSec, 0);

		vPref = vPref.GetClampedToMaxSize2D(UKismetMathLibrary::FMin(v * deltaSec, FVector2D::Distance(to2D(agent->GetActorLocation()), target)));

		FVector oldVel = velocity;

		if (avoidAgents) {
			adjustVelocity(to2D(vPref), deltaSec);
			//velocity = vPref;
		} else {
			velocity = vPref;
		}

		FVector2D q = to2D(velocity - oldVel);
		float dv = FMath::Sqrt(FVector2D::DotProduct(q, q));

		if (dv > aMax * deltaSec) {
			float f = aMax * deltaSec / dv;
			velocity = (1 - f) * oldVel + f * velocity;
		}

		float rot = agent->GetActorRotation().Yaw;

		if (velocity.Size2D() != 0) {
			if (angleDiff(rot, velocity.Rotation().Yaw) > 90) {
				velocity = -velocity;
				setRotation();
				velocity = -velocity;
			} else {
				setRotation();
			}
		}

		/*
		if (UKismetMathLibrary::Abs(angleDiff(rot, agent->GetActorRotation().Yaw)) / deltaSec > maxAngle * (v / L) + 5) {
			GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("rot: %f yaw: %f -> %f (%f)      %d   %s -> %s"), rot, agent->GetActorRotation().Yaw, angleDiff(rot, agent->GetActorRotation().Yaw), UKismetMathLibrary::Abs(angleDiff(rot, agent->GetActorRotation().Yaw)) / deltaSec, vPref.Equals(velocity), *to2D(vPref).ToString(), *to2D(velocity).ToString()));
			DrawDebugLine(GWorld->GetWorld(), FVector(to2D(agent->GetActorLocation()), 20), FVector(to2D(agent->GetActorLocation()), 30), FColor::Red, false, 0.5, 0, 1);
			GEngine->DeferredCommands.Add(TEXT("pause"));
		}*/

		agent->SetActorLocation(agent->GetActorLocation() + velocity);

		/*
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
		*/
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
	return 5000;
	return vMax * vMax / (aMax * 2);
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

FVector2D ADynamicCarController::vSample(float deltaSec)
{
	FVector2D vCand;
	float curRot, newRot, d;

	do {
		do {
			vCand = FVector2D(2.0f*rand() - RAND_MAX, 2.0f*rand() - RAND_MAX);
		} while (FVector2D::DotProduct(vCand, vCand) > (((float)RAND_MAX) * ((float)RAND_MAX)));

		vCand *= (vMax / RAND_MAX) * deltaSec;

		curRot = agent->GetActorRotation().Yaw;
		curRot = positiveAngle(curRot);

		newRot = to3D(vCand).Rotation().Yaw;
		newRot = positiveAngle(newRot);

		d = FMath::Abs(newRot - curRot);

		if (d > 90) {
			// We cannot drive forward in this direction, can we drive backwards?

			newRot += 180;
			newRot = positiveAngle(newRot);

			d = FMath::Abs(newRot - curRot);
		}

	} while (d > maxAngle * deltaSec);

	return vCand;
}