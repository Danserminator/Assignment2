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
				float bestC = 1;
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

				float prevRot = agent->GetActorRotation().Yaw;

				prevRot = positiveAngle(prevRot);

				if (velocity == FVector::ZeroVector) {
					float r = 2.0f*rand() - RAND_MAX;
					float rot = agent->GetActorRotation().Yaw;
					rot += r * (maxAngle / RAND_MAX) * deltaSec;
					agent->SetActorRotation(FRotator(0, rot, 0));

					rot = agent->GetActorRotation().Yaw;

					rot = positiveAngle(rot);

					if (FMath::Abs(rot - prevRot) > maxAngle * deltaSec) {
						bool isPref = velocity.Equals(to3D(vPref));
						GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Rotation: %f, Max rotation: %f, Is pref: %d"), FMath::Abs(prevRot - rot) / deltaSec, maxAngle, isPref));
					}
				} else {
					float tarRot = velocity.Rotation().Yaw;
					tarRot = positiveAngle(tarRot);

					float curRot = agent->GetActorRotation().Yaw;
					curRot = positiveAngle(curRot);

					if (UKismetMathLibrary::Abs(tarRot - curRot) > 90) {
						velocity = -velocity;
						setRotation();
						velocity = -velocity;

						float rot = agent->GetActorRotation().Yaw;

						rot = positiveAngle(rot);

						if (FMath::Abs(rot - prevRot) > maxAngle * deltaSec + 0.1) {
							bool isPref = velocity.Equals(to3D(vPref));
							GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Yellow, FString::Printf(TEXT("Rotation: %f, Max rotation: %f, Is pref: %d"), FMath::Abs(prevRot - rot) / deltaSec, maxAngle, isPref));
						}
					} else {
						setRotation();

						float rot = agent->GetActorRotation().Yaw;

						rot = positiveAngle(rot);

						if (FMath::Abs(rot - prevRot) > maxAngle * deltaSec + 0.1) {
							bool isPref = velocity.Equals(to3D(vPref));
							DrawDebugLine(GWorld->GetWorld(), agent->GetActorLocation(), agent->GetActorLocation() + collisionSize, FColor::Yellow, false, 2, 0, 1);
							GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Rotation: %f, Max rotation: %f, Is pref: %d"), FMath::Abs(prevRot - rot) / deltaSec, maxAngle, isPref));
						}
					}
				}

				if (velocity.Size2D() > vMax * deltaSec + 0.1) {
					GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Blue, FString::Printf(TEXT("Velocity: %f, Max velocity: %f"), velocity.Size2D() / deltaSec, vMax));
				}

				agent->SetActorLocation(loc + velocity);
			}
			else {
				if (rotating) {
					rotating = !rotate(deltaSec);
				}
				else {
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

	if (newVelocity.Size2D() > remainingDistance.Size()) {
		newVelocity = newVelocity.GetClampedToSize2D(-UKismetMathLibrary::Abs(remainingDistance.Size()), UKismetMathLibrary::Abs(remainingDistance.Size()));
	}

	return newVelocity;
}

FVector ADifferentialDriveController::getBestVelocity(float deltaSec)
{
	float tarRot = getRotation(agent->GetActorLocation(), target);
	float curRot = agent->GetActorRotation().Yaw;

	tarRot = positiveAngle(tarRot);
	curRot = positiveAngle(curRot);

	float currentMaxAngle = maxAngle * deltaSec;

	float cRot;

	if (UKismetMathLibrary::Abs(tarRot - curRot) > 90) {
		// We will drive backwards!

		cRot = UKismetMathLibrary::ClampAngle(tarRot - curRot, 180 - currentMaxAngle, 180 + currentMaxAngle);
		
	} else {
		// We will drive forwards!

		cRot = UKismetMathLibrary::ClampAngle(tarRot - curRot, -currentMaxAngle, currentMaxAngle);
	}

	cRot = positiveAngle(cRot);
	cRot += curRot;

	cRot = positiveAngle(cRot);

	FVector bestVel(0, 0, 0);
	bestVel.X = vMax * UKismetMathLibrary::DegCos(cRot) * deltaSec;
	bestVel.Y = vMax * UKismetMathLibrary::DegSin(cRot) * deltaSec;

	FVector2D remainingDistance = target - to2D(agent->GetActorLocation());

	if (bestVel.Size2D() > remainingDistance.Size()) {
		bestVel = bestVel.GetClampedToSize2D(-UKismetMathLibrary::Abs(remainingDistance.Size()), UKismetMathLibrary::Abs(remainingDistance.Size()));
	}

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

		if (test > 90) {
			// We cannot drive forward in this direction, can we drive backwards?

			newRot += 180;
			newRot = positiveAngle(newRot);

			test = FMath::Abs(newRot - curRot);
		}

	} while (test > maxAngle * deltaSec);

	return vCand;
}