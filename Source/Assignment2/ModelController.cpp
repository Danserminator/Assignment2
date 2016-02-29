// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "ModelController.h"

//#define OUTPUT

/*
void AModelController::BeginPlay()
{
	agent = static_cast<AAgent *>(GetPawn());	// Check if can be set in constructor.

	R = agent->R;
	formation = agent->formation;
	unseenAgents = agent->unseenAgents;
}
*/


// Called every frame
void AModelController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	drawLine(5 * velocity, velocityColor);

	if (searching) {
		if (stopped) {
			DrawDebugCircle(GWorld->GetWorld(), agent->GetActorLocation(), R, radiusSegments, radiusColor,
							false, 0.1, 0, 1, FVector(0, 1, 0), FVector(1, 0, 0), false);
		} else {
			DrawDebugPoint(GWorld->GetWorld(), to3D(target), searchSize, searchColor, false, 0.1, 0);
		}
	}
}

void AModelController::setWaypoints(AVisibilityGraph * graph, TArray<FVector2D> customers)
{
	waypoints = TArray<FVector2D>();

	if (customers.Num() != 0) {
		waypoints = AStar::getPath(graph->getGraph(), graph->getVertices(), getClosest(graph->getVertices(), to2D(agent->GetActorLocation())), customers[0]);
	}
	
	for (int32 c = 1; c < customers.Num(); c++) {
		waypoints.Append(AStar::getPath(graph->getGraph(), graph->getVertices(), customers[c - 1], customers[c]));
	}

	writeWaypointsToFile("Waypoints.txt");
}

FVector2D AModelController::getClosest(TArray<FVector2D> positions, FVector2D position)
{
	int32 closestIndex = 0;
	float closestDistance = std::numeric_limits<float>::infinity();

	for (int32 c = 0; c < positions.Num(); c++) {
		float distance = FVector2D::Distance(positions[c], position);

		if (distance < closestDistance) {
			closestDistance = distance;
			closestIndex = c;
		}
	}

	return positions[closestIndex];
}

void AModelController::setTarget() {
	findNewAgents();

	if (unseenAgents.Num() > 0) {							// Approach target if there are still unseen agents
		searching = true;
		target = approachAgents();
	} else {												// Otherwise move towards assigned position in formation
		if (stopped) {
			try {											// Get target from formation after finding all agents and stationary
				target = formation->getTarget(formationPosition);

				FVector2D fV = formation->getVelocity();
				if (fV.X + fV.Y != 0) {						// TODO: > epsilon?
					//adjustTarget(fV);
				}

				searching = false;
			} catch (std::exception e) {
				target = to2D(agent->GetActorLocation());	// Remain stationary until all agents have found each other
			}
		} else {
			target = getBrakeTarget();
		}
	}
}

void AModelController::findNewAgents()
{
	if (unseenAgents.Num() > 0) {							// Do nothing if all agents are already found
		FVector2D curLoc = to2D(agent->GetActorLocation());

		if (R <= 0) {										// R < 0 means all agents know about all other agents already
			seenAgents = unseenAgents;
			unseenAgents.Empty();
		} else {
			float dist;
			for (int32 c = 0; c < unseenAgents.Num(); c++) {
				dist = FVector2D::Distance(curLoc, to2D(unseenAgents[c]->GetActorLocation()));
				if (dist <= R) {							// If agent is withing radius
					if (unseenAgents[c]->GetActorLocation() != agent->GetActorLocation()) {
						seenAgents.Add(unseenAgents[c]);	// Dont add oneself to seen agents
					}

					unseenAgents.RemoveAt(c);
					c--;
				}
			}
		}
		
		if (unseenAgents.Num() == 0) {						// If all agents are found, signal formation
			formationPosition = formation->foundAllAgents(curLoc);
			if (R > 0) {									// Stop moving after all agents have been found
				stopped = false;
			}
		}
	}
}

FVector2D AModelController::approachAgents()
{
	FVector2D goal;
	for (int32 c = 0; c < seenAgents.Num(); c++) {
		goal += to2D(seenAgents[c]->GetActorLocation() - agent->GetActorLocation());
	}

	return goal;
}

void AModelController::adjustTarget(FVector2D formationVelocity)
{
	// http://twobitcoder.blogspot.se/2010/04/circle-collision-detection.html

	FVector2D vAB = formationVelocity - to2D(velocity);
	FVector2D pAB = to2D(formation->GetActorLocation() - agent->GetActorLocation());

	float a = FVector2D::DotProduct(vAB, vAB);
	float b = 2 * FVector2D::DotProduct(pAB, vAB);
	float c = FVector2D::DotProduct(pAB, pAB);

	float discriminant = (b*b) - (4 * a * c);
}

FVector2D AModelController::getBrakeTarget()
{
	stopped = true;
	return to2D(agent->GetActorLocation());
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

void AModelController::writeWaypointsToFile(const FString fileName)
{
	FString str;
	str.Append(FString("-1\t-1"));
	for (int32 i = 0; i < waypoints.Num(); i++) {
		str.Append("\r\n");
		str.Append(FString::SanitizeFloat(waypoints[i].Y));
		str.Append("\t");
		str.Append(FString::SanitizeFloat(waypoints[i].X));
	}
	str.Append("\r\n");

	FString projectDir = FPaths::GameDir();
	projectDir += "Output Data/" + fileName;

	FString stored;
	
	if (FFileHelper::LoadFileToString(stored, *projectDir)) {
		stored.Append(str);
	} else {
		stored = str;
	}

	FFileHelper::SaveStringToFile(stored, *projectDir);
}