// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "KinematicCarController.h"

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

			waypoints = DubinsPath::getPath(waypoints, to2D(agent->GetActorLocation()), rotation.Yaw, maxAngle, L, graph, errorTolerance);

			writeWaypointsToFile("Waypoints2.txt");

			if (waypoints.Num() > 0) {
				target = waypoints[0];
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

		velocity = FVector(vMax * UKismetMathLibrary::DegCos(rotation) * deltaSec, vMax * UKismetMathLibrary::DegSin(rotation) * deltaSec, 0);

		velocity = velocity.GetClampedToMaxSize2D(UKismetMathLibrary::FMin(vMax, FVector2D::Distance(to2D(agent->GetActorLocation()), target)));

		float rot = agent->GetActorRotation().Yaw;

		setRotation();

		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("%f"), UKismetMathLibrary::Abs(rot - agent->GetActorRotation().Yaw) / deltaSec));

		agent->SetActorLocation(agent->GetActorLocation() + velocity);

		DrawDebugLine(GWorld->GetWorld(), agent->GetActorLocation(), agent->GetActorLocation() + collisionSize, FColor::Green, false, 0.1, 0, 1);
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
