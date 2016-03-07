// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicCarController.h"

AKinematicCarController::AKinematicCarController()
{
	errorTolerance = 3;
	vMax = kccVMax;
	aMax = 2 * vMax;
}

void AKinematicCarController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
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
			velocity = FVector(0, 0, 0);

			bool t35 = followPath && waypointsIndex >= waypoints.Num();
			bool t4 = avoidAgents && !followPath;

			if (t35 || t4) {
				play = false;
				GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Time: %f\r\n"), totalTime));
			}

			return;
		}

		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		float rotation = rotate(deltaSec);

		FVector vPref = FVector(vMax * UKismetMathLibrary::DegCos(rotation) * deltaSec, vMax * UKismetMathLibrary::DegSin(rotation) * deltaSec, 0);
		
		vPref = vPref.GetClampedToMaxSize2D(UKismetMathLibrary::FMin(vMax * deltaSec, FVector2D::Distance(to2D(agent->GetActorLocation()), target)));

		if (avoidAgents) {
			adjustVelocity(to2D(vPref), deltaSec);
			//velocity = vPref;
		} else {
			velocity = vPref;
		}

		float rot = agent->GetActorRotation().Yaw;

		if (velocity.Size2D() != 0) {
			setRotation();
		}

		if (UKismetMathLibrary::Abs(angleDiff(rot, agent->GetActorRotation().Yaw)) / deltaSec > maxAngle * (vMax / L) + 5) {
			GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("rot: %f yaw: %f -> %f [%f]"), rot, agent->GetActorRotation().Yaw, angleDiff(rot, agent->GetActorRotation().Yaw), UKismetMathLibrary::Abs(angleDiff(rot, agent->GetActorRotation().Yaw)) / deltaSec));
		}

		agent->SetActorLocation(agent->GetActorLocation() + velocity);

		//DrawDebugLine(GWorld->GetWorld(), agent->GetActorLocation(), agent->GetActorLocation() + collisionSize, FColor::Green, false, 0.1, 0, 1);
	}
}

float AKinematicCarController::rotate(float deltaSec) const
{
	float rotation = positiveAngle(AModelController::getRotation(agent->GetActorLocation(), target));

	rotation -= agent->GetActorRotation().Yaw;

	float curMaxAngle = maxAngle * deltaSec;					// Max angle for this tick

	rotation = UKismetMathLibrary::ClampAngle(positiveAngle(rotation), -curMaxAngle, curMaxAngle);

	rotation = UKismetMathLibrary::DegTan(positiveAngle(rotation));

	rotation *= vMax / L;
 
	rotation += UKismetMathLibrary::DegreesToRadians(agent->GetActorRotation().Yaw);

	return UKismetMathLibrary::RadiansToDegrees(rotation);
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

FVector2D AKinematicCarController::vSample(float deltaSec)
{
	FVector2D vCand;

	float tVMax = vMax * ((double)rand() / RAND_MAX);

	//(2 * ((double)rand() / RAND_MAX) - 1)

	float r = ((2.0f*rand() - RAND_MAX) / RAND_MAX);
	float rotation = maxAngle * deltaSec * r;

	if (rotation > maxAngle * deltaSec || rotation < -maxAngle * deltaSec) {
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("%f * %f * %f = %f"), maxAngle, deltaSec, r, rotation));
	}

	rotation = UKismetMathLibrary::DegTan(positiveAngle(rotation));

	rotation *= vMax / L;

	rotation += UKismetMathLibrary::DegreesToRadians(agent->GetActorRotation().Yaw);

	rotation = UKismetMathLibrary::RadiansToDegrees(rotation);

	vCand = FVector2D(vMax * UKismetMathLibrary::DegCos(rotation) * deltaSec, vMax * UKismetMathLibrary::DegSin(rotation) * deltaSec);

	vCand = to2D(to3D(vCand).GetClampedToMaxSize2D(vMax));

	/*
	FVector nCand;
	float curRot;
	float newRot;
	float test;
	do {
		do {
			vCand = FVector2D(2.0f*rand() - RAND_MAX, 2.0f*rand() - RAND_MAX);
		} while (FVector2D::DotProduct(vCand, vCand) > (((float)RAND_MAX) * ((float)RAND_MAX)));

		vCand *= (vMax / RAND_MAX) * deltaSec;

		nCand = to3D(vCand);
		nCand.Normalize();

		curRot = agent->GetActorRotation().Yaw;
		curRot = positiveAngle(curRot);

		newRot = to3D(vCand).Rotation().Yaw;
		newRot = positiveAngle(newRot);

		test = FMath::Abs(newRot - curRot);
	} while (test > maxAngle * deltaSec);
	*/

	return vCand;
}