// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "ModelController.h"

//#define OUTPUT

void AModelController::BeginPlay()
{
	agent = static_cast<AAgent *>(GetPawn());	// Check if can be set in constructor.
}

// Called every frame
void AModelController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	drawLine(2 * velocity, velocityColor);

	if (searching) {
		if (stopped) {
			DrawDebugCircle(GWorld->GetWorld(), agent->GetActorLocation(), agent->getSeeRadius(), radiusSegments, radiusColor,
							false, 0.1, 0, 1, FVector(0, 1, 0), FVector(1, 0, 0), false);
		} else {
			DrawDebugPoint(GWorld->GetWorld(), to3D(target), searchSize, searchColor, false, 0.1, 0);
		}
	}
}

void AModelController::setWaypoints(AVisibilityGraph * graph, TArray<FVector2D> customers)
{
	followPath = true;

	waypoints = TArray<FVector2D>();

	if (customers.Num() != 0) {
		waypoints = AStar::getPath(graph->getGraph(), graph->getVertices(), getClosest(graph->getVertices(), to2D(agent->GetActorLocation())), customers[0]);
	} else {
		waypoints.Add(to2D(agent->GetActorLocation()));
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

void AModelController::setParameters(AFormation * formation, bool followPath, bool movingFormation)
{
	this->formation = formation;
	this->followPath = followPath;
	this->movingFormation = movingFormation;
}

bool AModelController::updateTarget()
{

	if (followPath) {
		return updateTarget_path();

	} else if (movingFormation) {
		return updateTarget_moving();

	} else {
		return updateTarget_still();
	}
}

bool AModelController::updateTarget_path()
{
	// The agent is following a predetermined path.
	everybodyKnows = true;

	bool reached = waypointReached(); // && velocity.Size() == 0;		// Check if it is at the target and is standing still
	if (reached) {
		waypointsIndex++;
	}
	if (waypointsIndex < waypoints.Num()) {
		target = waypoints[waypointsIndex];
	}

	return reached;
}

bool AModelController::updateTarget_moving()
{

	FVector2D oldTarget = target;

	// The agent is following a moving formation.

	agent->findAgents();	// Check if we can see any new agents.

	if (agent->numberUnseenAgents() > 0) {
		// We cannot see all the other agents so move towards the agents we can see.
		searching = true;
		target = approachAgents();

	} else {
		// We know where all the other agents are.

		formation->foundAllAgents(agent);	// Tell formation that we have found all agents.

		try {
			target = formation->getTarget(agent);
		}
		catch (std::exception e) {
			// At least one of the other agents do not know where everybody else are.
			// I will move towards all the other agents.
			target = approachAgents();

			return false;
		}
	}

	return true;
}

bool AModelController::updateTarget_still()
{
	FVector2D oldTarget = target;

	// The agent is moving towards a stationary formation.

	agent->findAgents();	// Check if we can see any new agents.

	if (agent->numberUnseenAgents() > 0) {
		// We cannot see all the other agents so move towards the agents we can see.
		searching = true;
		target = approachAgents();
		everybodyKnows = false;

		//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Position: %s -> %s"), *agent->GetActorLocation().ToString(), *target.ToString()));

	} else {
		// We know where all the other agents are.

		formation->foundAllAgents(agent);	// Tell formation that we have found all agents.

		searching = false;

		try {
			target = formation->getTarget(agent);

			everybodyKnows = true;

			if (AModelController::waypointReached() && velocity.Size() < 0.2) {

			} else {

				// Check if I am moving towards the target right now, else stop.
				if (!isMovingTowardsTarget(target)) {
					// Not moving towards the target == we should be stopping.

					target = to2D(agent->GetActorLocation()) - to2D(velocity); // to2D(agent->GetActorLocation());	// Move towards myself.
				}
			}

		} catch (std::exception e) {
			// At least one of the other agents do not know where everybody else are.
			// I will move towards all the other agents.
			target = approachAgents();
			everybodyKnows = false;

			return false;
		}
	}

	return true;
}

bool AModelController::isMovingTowardsTarget(FVector2D target)
{
	if (velocity.Size() == 0) {
		// We are standing still.
		return true;
	}

	FVector2D movingTowards = to2D(velocity);
	movingTowards.Normalize();

	//float rotation = getRotation(agent->GetActorLocation(), target).Yaw;

	//FVector2D shouldBeMovingTowards = FVector2D(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation));

	FVector2D shouldBeMovingTowards = target - to2D(agent->GetActorLocation());

	shouldBeMovingTowards.Normalize();

	return movingTowards.Equals(shouldBeMovingTowards, 0.3);
}

FVector2D AModelController::approachAgents()
{
	TArray<AAgent *> agents = agent->getSeenAgents();

	FVector2D goal;
	for (int32 c = 0; c < agents.Num(); c++) {
		goal += to2D(agents[c]->GetActorLocation() - agent->GetActorLocation());
	}

	return goal;
}

FVector2D AModelController::getBrakeTarget()
{
	stopped = true;
	return to2D(agent->GetActorLocation());
}

bool AModelController::waypointReached()
{
	FVector2D currentLocation = to2D(agent->GetActorLocation());

	bool reached = currentLocation.Equals(target, errorTolerance);

	if (reached) {
		stopped = true;
	}

	return reached;
}

float AModelController::getRotation(FVector start, FVector target) const
{
	return UKismetMathLibrary::FindLookAtRotation(start, target).Yaw;
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

/*
bool AModelController::setTarget()
{
if (!followPath) {
findNewAgents();

if (unseenAgents.Num() > 0) {							// Approach target if there are still unseen agents
searching = true;
target = approachAgents();
} else {												// Otherwise move towards assigned position in formation
if (stopped) {
try {											// Get target from formation after finding all agents and stationary
target = formation->getTarget(formationPosition);

FVector2D fV = formation->getVelocity();
if (!movingFormation && fV.X + fV.Y != 0) {	// TODO: > epsilon?
movingFormation = true;
stopped = false;
//adjustTarget(fV);
}

searching = false;
}
catch (std::exception e) {
target = to2D(agent->GetActorLocation());	// Remain stationary until all agents have found each other
}
}
else {
if (!movingFormation) {
target = getBrakeTarget();
}
}
}
} else {
bool reached = waypointReached();
if (reached) {
waypointsIndex++;
}
if (waypointsIndex < waypoints.Num()) {
target = waypoints[waypointsIndex];
}

return reached;
}

//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Position: %s -> %s"), *agent->GetActorLocation().ToString(), *target.ToString()));

return false;
}
*/