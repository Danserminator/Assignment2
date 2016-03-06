// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicPointController.h"


#define OUTPUT

AKinematicPointController::AKinematicPointController()
{
	errorTolerance = 0.001;
	vMax = kpcVMax;
	aMax = 2 * vMax;
}

void AKinematicPointController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
		updateTarget();

		if (waypointReached()) {
			// TODO: Vet inte varför koden inte fungerar här
		} else {
			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			FVector vPref = getVelocity(deltaSec);

			if (avoidAgents) {
				adjustVelocity(to2D(vPref), deltaSec);
			} else {
				velocity = vPref;
			}

			//checkObstacles(deltaSec);

			FVector currentLocation = agent->GetActorLocation();

			FVector newLocation = currentLocation + velocity;

			setRotation();

			agent->SetActorLocation(newLocation);
			//agent->SetActorLocationAndRotation(newLocation, rotation);

			if (waypointReached()) {
				bool t1 = !followPath && !movingFormation && !avoidAgents;
				bool t35 = followPath && waypointsIndex >= waypoints.Num();
				bool t4 = avoidAgents && !followPath;

				if (t1 || t35 || t4) {
					play = false;
					GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Time: %f\r\n"), totalTime));
				}
			}
		}
	}
}

FVector AKinematicPointController::getVelocity(float deltaSec) const
{
	FVector newVelocity;

	float rotation = getRotation(agent->GetActorLocation(), target);

	newVelocity = deltaSec * vMax * FVector(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation), 0);

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = UKismetMathLibrary::Abs(remainingDistance.X);
	remainingDistance.Y = UKismetMathLibrary::Abs(remainingDistance.Y);

	newVelocity.X = UKismetMathLibrary::FClamp(newVelocity.X, -remainingDistance.X, remainingDistance.X);
	newVelocity.Y = UKismetMathLibrary::FClamp(newVelocity.Y, -remainingDistance.Y, remainingDistance.Y);

	return newVelocity;
}

FVector2D AKinematicPointController::vSample(float deltaSec)
{
	FVector2D vCand;
	do {
		vCand = FVector2D(2.0f*rand() - RAND_MAX, 2.0f*rand() - RAND_MAX);
	} while (FVector2D::DotProduct(vCand, vCand) > (((float)RAND_MAX) * ((float)RAND_MAX)));

	vCand *= (vMax / RAND_MAX) * deltaSec;

	return vCand;
}

/*
void AKinematicPointController::checkObstacles(float deltaSec)
{
	TArray<AAgent *> agents = agent->getSeenAgents();

	for (int32 c = 0; c < agents.Num(); c++) {
		float dist = FVector2D::Distance(to2D(agent->GetActorLocation()), to2D(agents[c]->GetActorLocation()));

		if (dist <= getSearchDistance()) {
			FVector2D collision = willCollide(agents[c]);

			if (collision.X > 0) {			// Will collide with agent
				/*
				if (collision.Y != 0) {		// Y != 0 -> Shift right
					float rot = agent->GetActorRotation().Yaw;
					rot += shiftAngle;
					agent->SetActorRotation(FRotator(0, rot, 0));
					velocity = getVelocity(deltaSec);
				} else {
					velocity = FVector(0, 0, 0);
					break;					// TODO: Ta bort?
				//}
			}
		}
	}
}*/