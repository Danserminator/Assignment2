// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DifferentialDriveController.h"

#define OUTPUT

ADifferentialDriveController::ADifferentialDriveController()
{
	errorTolerance = 0.001;
	vMax = ddcVMax;
}

void ADifferentialDriveController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (play) {
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

			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();
			
			if (avoidAgents) {
				FVector bestVel = getBestVelocity(deltaSec);

				float distToGoal = FVector2D::Distance(target, to2D(agent->GetActorLocation()));

				float bestDist = distToGoal;
				float bestC = 0;
				FVector loc = agent->GetActorLocation();
				for (float c = 0.1; c <= 1; c += 0.1) {
					FVector tLoc = loc + c * bestVel;
					float tDist = FVector2D::Distance(target, to2D(tLoc));

					if (tDist < bestDist) {
						bestDist = tDist;
						bestC = c;
					}
				}

				FVector2D vPref = to2D(bestVel) * bestC;

				adjustVelocity(vPref, deltaSec);
				//velocity = to3D(vPref);

				float f = 0;
				float prevRot = agent->GetActorRotation().Yaw;
				prevRot = UKismetMathLibrary::FMod((prevRot + 360), 360, f);

				if (velocity == FVector::ZeroVector) {
					float r = 2.0f*rand() - RAND_MAX;
					float rot = agent->GetActorRotation().Yaw;
					rot +=  r * maxAngle * deltaSec;
					agent->SetActorRotation(FRotator(0, rot, 0));

					rot = UKismetMathLibrary::FMod((rot + 360), 360, f);

					if (FMath::Abs(prevRot - rot) > maxAngle * deltaSec) {
						bool isPref = velocity.Equals(to3D(vPref));
						GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("%f vSize: %f, %d\r\n"), FMath::Abs(prevRot - rot) / deltaSec, velocity.Size2D() / deltaSec, isPref));
					}
				} else {
					setRotation();

					float rot = agent->GetActorRotation().Yaw;
					rot = UKismetMathLibrary::FMod((rot + 360), 360, f);

					if (FMath::Abs(prevRot - rot) > maxAngle * deltaSec + 0.1) {
						bool isPref = velocity.Equals(to3D(vPref));
						DrawDebugLine(GWorld->GetWorld(), agent->GetActorLocation(), agent->GetActorLocation() + collisionSize, FColor::Green, false, 0.1, 0, 1);
						GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("%f vSize: %f, %d\r\n"), FMath::Abs(prevRot - rot) / deltaSec, velocity.Size2D() / deltaSec, isPref));
					}
				}
				//agent->SetActorRotation(FRotator(0, velocity.Rotation().Yaw, 0));

				agent->SetActorLocation(loc + velocity);
			} else {
				if (rotating) {
					rotating = !rotate(deltaSec);
				} else {
					velocity = getVelocity(deltaSec);

					FVector currentLocation = agent->GetActorLocation();

					FVector newLocation = currentLocation + velocity;

					agent->SetActorLocation(newLocation);
				}
			}
		}
	}
}

bool ADifferentialDriveController::rotate(float deltaSec)
{
	float rotation = getRotation(agent->GetActorLocation(), target);

	rotation -= agent->GetActorRotation().Yaw;

	float currentMaxAngle = maxAngle * deltaSec;

	float clampedRotation = UKismetMathLibrary::ClampAngle(rotation, -currentMaxAngle, currentMaxAngle);

	agent->SetActorRotation(FRotator(0, agent->GetActorRotation().Yaw + clampedRotation, 0));

	return abs(rotation - clampedRotation) < 0.0001;
}

FVector ADifferentialDriveController::getVelocity(float deltaSec)
{
	FVector newVelocity(0, 0, 0);

	if (!rotate(deltaSec)) {
		rotating = true;
		return FVector(0, 0, 0);	// Still need to turn
	}

	newVelocity.X = getXVelocity(deltaSec);

	newVelocity.Y = getYVelocity(deltaSec);

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = UKismetMathLibrary::Abs(remainingDistance.X);
	remainingDistance.Y = UKismetMathLibrary::Abs(remainingDistance.Y);

	newVelocity.X = UKismetMathLibrary::FClamp(newVelocity.X, -remainingDistance.X, remainingDistance.X);
	newVelocity.Y = UKismetMathLibrary::FClamp(newVelocity.Y, -remainingDistance.Y, remainingDistance.Y);

	return newVelocity;
}

FVector ADifferentialDriveController::getBestVelocity(float deltaSec)
{
	float tarRot = getRotation(agent->GetActorLocation(), target);
	float curRot = agent->GetActorRotation().Yaw;

	float currentMaxAngle = maxAngle * deltaSec;
	float cRot = UKismetMathLibrary::ClampAngle(tarRot - curRot, -currentMaxAngle, currentMaxAngle);
	cRot += curRot;

	FVector bestVel;
	bestVel.X = vMax * UKismetMathLibrary::DegCos(cRot) * deltaSec;
	bestVel.Y = vMax * UKismetMathLibrary::DegSin(cRot) * deltaSec;

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());
	remainingDistance.X = UKismetMathLibrary::Abs(remainingDistance.X);
	remainingDistance.Y = UKismetMathLibrary::Abs(remainingDistance.Y);

	bestVel.X = UKismetMathLibrary::FClamp(bestVel.X, -remainingDistance.X, remainingDistance.X);
	bestVel.Y = UKismetMathLibrary::FClamp(bestVel.Y, -remainingDistance.Y, remainingDistance.Y);

	return bestVel;
}

float ADifferentialDriveController::getXVelocity(float deltaSec) const
{
	return deltaSec * vMax * UKismetMathLibrary::DegCos(agent->GetActorRotation().Yaw);
}

float ADifferentialDriveController::getYVelocity(float deltaSec) const
{
	return deltaSec * vMax * UKismetMathLibrary::DegSin(agent->GetActorRotation().Yaw);
}

FVector2D ADifferentialDriveController::vSample(float deltaSec)
{
	FVector2D vCand;
	FVector nCand;
	do {
		do {
			vCand = FVector2D(2.0f*rand() - RAND_MAX, 2.0f*rand() - RAND_MAX);
		} while (FVector2D::DotProduct(vCand, vCand) > (((float)RAND_MAX) * ((float)RAND_MAX)));

		vCand *= (vMax / RAND_MAX) * deltaSec;

		nCand = to3D(vCand);
		nCand.Normalize();
	} while (FMath::Abs(UKismetMathLibrary::DegAtan2(nCand.Y, nCand.X) - agent->GetActorRotation().Yaw) > maxAngle * deltaSec);

	return vCand;
}