// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "ModelController.h"

//#define OUTPUT

// Called every frame
void AModelController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AModelController::setTarget() {
	findNewAgents();

	// Approach target if there are still unseen agents
	if (unseenAgents.Num() > 0) {
		target = approachAgents();
	} else {	// Otherwise move towards assigned position in formation
		try {
			target = formation->getTarget(formationPosition);
		} catch (std::exception e) {	// Stand still if not all agents have found all other agents yet
			target = to2D(agent->GetActorLocation());	// TODO: Kanske borde göra approachAgents() en-några gånger till?
		}
	}
}

void AModelController::findNewAgents()
{
	if (unseenAgents.Num() > 0) {	// Do nothing if all agents are already found
		FVector2D curLoc = to2D(agent->GetActorLocation());

		if (R < 0) {				// R < 0 means all agents know about all other agents already
			seenAgents = unseenAgents;
			unseenAgents.Empty();
		} else {
			float dist;
			for (int32 c = 0; c < unseenAgents.Num(); c++) {
				dist = FVector2D::Distance(curLoc, to2D(unseenAgents[c]->GetActorLocation()));
				if (dist <= R) {
					seenAgents.Add(unseenAgents[c]);
					unseenAgents.RemoveAt(c);
					c--;
				}
			}
		}
		
		if (unseenAgents.Num() == 0) {	// If all agents are found, signal formation
			formationPosition = formation->foundAllAgents(curLoc);
		}
	}
}

FVector2D AModelController::approachAgents()
{
	return FVector2D();
}

bool AModelController::waypointReached()
{
#ifdef OUTPUT
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Test")));
#endif

	FVector2D currentLocation = to2D(agent->GetActorLocation());

	return currentLocation.Equals(target, errorTolerance);
}

FRotator AModelController::getRotation(FVector start, FVector target) const
{
	return FRotator(0, UKismetMathLibrary::FindLookAtRotation(start, target).Yaw, 0);
}

void AModelController::setRotation()
{
	FVector normVelocity = velocity;
	normVelocity.Normalize();

	float yaw = UKismetMathLibrary::DegAtan2(normVelocity.Y, normVelocity.X);

	agent->SetActorRotation(FRotator(0, yaw, 0));
}

void AModelController::drawLine(FVector line, FColor color) const
{
	FVector start = agent->GetActorLocation();
	DrawDebugLine(GWorld->GetWorld(), start, start + line, color, false, 0.1, 0, 1);
}

FVector2D AModelController::to2D(FVector vector) const
{
	return FVector2D(vector.X, vector.Y);
}

FVector AModelController::to3D(FVector2D vector) const
{
	return FVector(vector, 0);
}