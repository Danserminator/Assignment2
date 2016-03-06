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

	if (play) {
		totalTime += DeltaTime;
	}

	drawLine(2 * velocity, velocityColor);

	if (avoidAgents) {
		updateNeighbours();

		FVector loc = FVector(agent->GetActorLocation().X, agent->GetActorLocation().Y, 0);
		DrawDebugCircle(GWorld->GetWorld(), loc, agent->getAgentRadius(), radiusSegments, radiusColor,
						false, 0.1, 0, 1, FVector(0, 1, 0), FVector(1, 0, 0), false);

		if (collided) {
			float rSqr = agent->getAgentRadius();
			rSqr *= rSqr;

			for (int32 c = 0; c < neighbours.Num(); c++) {
				float distSqr = neighbours[c].Get<0>();

				if (distSqr > rSqr) break;
				else {
					play = false;
					FVector aLoc = agent->GetActorLocation();
					DrawDebugLine(GWorld->GetWorld(), aLoc, aLoc + collisionSize, collisionColor, false, 0.1, 0, 1);
				}
			}

			/*
			for (int32 c = 0; c < agents.Num(); c++) {
				FVector2D aLoc = to2D(agent->GetActorLocation());
				FVector2D oLoc = to2D(agents[c]->GetActorLocation());
				float dist = FVector2D::Distance(aLoc, oLoc);
				//float dist = FVector2D::DotProduct(aLoc - oLoc, aLoc - oLoc);

				if (dist < agent->getAgentRadius()) {
					play = false;
					FVector cLoc = FVector(((aLoc + oLoc) / 2), 0);
					DrawDebugLine(GWorld->GetWorld(), cLoc, cLoc + collisionSize, collisionColor, false, 0.1, 0, 1);
				}
			}
			*/
		}
	}

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
	this->graph = graph;

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

void AModelController::setGoal(FVector2D goal)
{
	target = goal;
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

void AModelController::setParameters(AFormation * formation, bool followPath, bool movingFormation, bool avoidAgents)
{
	this->formation = formation;
	this->followPath = followPath;
	this->movingFormation = movingFormation;
	this->avoidAgents = avoidAgents;

	if (agent->numberUnseenAgents() == 0) {
		formation->foundAllAgents(agent);	// Tell formation that we have found all agents.
	}
}

bool AModelController::updateTarget()
{
	if (followPath) {
		return updateTarget_path();
	} else if (movingFormation) {
		return updateTarget_moving();
	} else if (!avoidAgents) {
		return updateTarget_still();
	}

	return false;
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

			FColor color = FColor::Red;

			everybodyKnows = true;

			if (AModelController::waypointReached() && velocity.Size() < 0.2) {

			} else {
				color = FColor::Blue;
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

	FVector2D goal(0, 0);
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

void AModelController::updateNeighbours()
{
	collided = false;
	neighbours.Empty();

	if (followPath) {
		computeObstacleNeighbours();
	}

	if (!collided) {
		computeAgentNeighbours();
	}

	neighbours.Sort([](const auto& One, const auto& Two) {
		return One.Get<0>() < Two.Get<0>();
	});

	/*
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("---------\r\n")));
	for (int32 c = 0; c < neighbours.Num(); c++) {
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("Element %d: %f\r\n"), c, neighbours[c].Get<0>()));
	}
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("---------\r\n")));
	*/
}

void AModelController::computeObstacleNeighbours()
{
	TArray<TArray<FVector2D>> edges = graph->getEdges();

	float rangeSq = getSearchDistance();
	FVector2D loc = to2D(agent->GetActorLocation());
	for (int32 c = 0; c < edges.Num(); c++) {
		float distSq;
		
		FVector2D ca = loc - edges[c][0];
		FVector2D cb = loc - edges[c][1];
		FVector2D ba = edges[c][1] - edges[c][0];
		float r = FVector2D::DotProduct(ca, ba) / FVector2D::DotProduct(ba, ba);

		if (r < 0) {								// point a is closest to c
			distSq = FVector2D::DotProduct(ca, ca);
		} else if (r > 1) {							// point b is closest to c
			distSq = FVector2D::DotProduct(cb, cb);
		} else {									// some point in between a and b is closest to c
			FVector2D q = loc - (edges[c][0] + r*(edges[c][1] - edges[c][0]));
			distSq = FVector2D::DotProduct(q, q);
		}

		if (distSq <= rangeSq) {
			float rSqr = agent->getAgentRadius() * agentRadiusScalar;
			rSqr *= rSqr;

			if (distSq < rSqr) {	// COLLISION!
				if (!collided) {
					collided = true;
					neighbours.Empty();
					rangeSq = rSqr;
				}

				neighbours.Emplace(distSq, OBSTACLE, c);
			} else if (!collided) {
				neighbours.Emplace(distSq, OBSTACLE, c);
			}
		}
	}
}

void AModelController::computeAgentNeighbours()
{
	TArray<AAgent *> agents = agent->getSeenAgents();

	for (int32 c = 0; c < agents.Num(); c++) {
		FVector2D q = to2D(agent->GetActorLocation() - agents[c]->GetActorLocation());
		float distSq = FVector2D::DotProduct(q, q);

		if (distSq <= getSearchDistance()) {
			float rSqr = agent->getAgentRadius() * agentRadiusScalar * 2;
			rSqr *= rSqr;

			if (distSq < rSqr) {
				if (!collided) {
					collided = true;
					neighbours.Empty();
				}

				neighbours.Emplace(distSq, AGENT, c);

				//FVector2D neighbour(AGENT, c);
				//neighbours.Add(dist, neighbour);
			} else if (!collided) {
				//FVector2D neighbour(AGENT, c);
				//neighbours.Add(dist, neighbour);
				neighbours.Emplace(distSq, AGENT, c);
			}
		}
	}
}

void AModelController::adjustVelocity(FVector2D vPref, float deltaSec)
{
	FVector newVelocity;

	float min_penalty = std::numeric_limits<float>::infinity();
	FVector2D vCand;

	for (int32 c = 0; c < vSamples; c++) {
		if (c == 0)	vCand = vPref;
		else		vCand = vSample(deltaSec);

		float dV;	// distance between candidate velocity and preferred velocity
		if (collided)	dV = 0;
		else {
			FVector2D q = vCand - vPref;
			dV = FMath::Sqrt(FVector2D::DotProduct(q, q));
		}

		float ct = std::numeric_limits<float>::infinity();	// time to collision
		for (int32 c = 0; c < neighbours.Num(); c++) {		//auto it = neighbours.CreateIterator(); it; ++it) {
			float ct_j = 0;									// time to collision with neighbour j
			FVector2D vab;

			int32 type = neighbours[c].Get<1>();			// type of neighbour
			int32 id = neighbours[c].Get<2>();				// id of neighbour (in respective list)

			FVector2D p = to2D(agent->GetActorLocation());
			float absSq = FVector2D::DotProduct(vCand, vCand);
			float r = agent->getAgentRadius() * agentRadiusScalar;

			if (type == AGENT) {
				AAgent * other = agent->getSeenAgents()[id];	// AAgent * other = (AAgent *) neighbours[c].Get<2>();

				AModelController * oController = static_cast<AModelController *>((other->GetController()));
				vab = 2 * vCand - to2D(velocity) - to2D(oController->velocity);

				FVector2D p2 = to2D(other->GetActorLocation());
				float time = timeToCollision(p, vab, p2, (r * 2), collided);

				if (collided) {
					ct_j = -FPlatformMath::CeilToFloat(time / deltaSec);
					ct_j -= absSq / (vMax * vMax);
				} else {
					ct_j = time;
				}
			} else if (type == OBSTACLE) {
				TArray<FVector2D> other = graph->getEdges()[id];
				//TArray<FVector2D> * other = (TArray<FVector2D> *) neighbours[c].Get<2>();

				float time_1 = timeToCollision(p, vCand, other[0], r, collided);
				float time_2 = timeToCollision(p, vCand, other[1], r, collided);
				float time_a = timeToCollision(p, vCand, other[0] + r * other[2], other[1] + r * other[2], collided);
				float time_b = timeToCollision(p, vCand, other[0] - r * other[2], other[1] - r * other[2], collided);

				if (collided) {
					float time = FMath::Max(FMath::Max(FMath::Max(time_1, time_2), time_a), time_b);
					ct_j = -FPlatformMath::CeilToFloat(time / deltaSec);
					ct_j -= absSq / (vMax * vMax);
				} else {
					float time = FMath::Min(FMath::Min(FMath::Min(time_1, time_2), time_a), time_b);
					if (time < deltaSec || (time * time) < absSq / (aMax * aMax)) {
						ct_j = time;
					} else {
						ct_j = std::numeric_limits<float>::infinity();
					}
				}
			}

			if (ct_j < ct) {
				ct = ct_j;

				if (safetyFactor / ct + dV >= min_penalty) {
					break;
				}
			}
		}

		float penalty = safetyFactor / ct + dV;
		if (penalty < min_penalty) {
			min_penalty = penalty;

			newVelocity = FVector(vCand, 0);
		}
	}

	velocity = newVelocity;
}

FVector2D AModelController::vSample(float deltaSec)
{
	return FVector2D();
}

// Time to collision of a ray to a disc
float AModelController::timeToCollision(FVector2D p, FVector2D v, FVector2D p2, float radius, bool collision) {
	FVector2D ba = p2 - p;
	float sq_diam = radius * radius;
	float time;

	float determinant = FVector2D::CrossProduct(v, ba);
	float absSq = FVector2D::DotProduct(v, v);

	float discriminant = -(determinant * determinant) + sq_diam * absSq;
	if (discriminant > 0) {
		if (collision) {
			time = (FVector2D::DotProduct(v, ba) + FMath::Sqrt(discriminant)) / absSq;
			if (time < 0) {
				time = -std::numeric_limits<float>::infinity();
			}
		} else {
			time = (FVector2D::DotProduct(v, ba) - FMath::Sqrt(discriminant)) / absSq;
			if (time < 0) {
				time = std::numeric_limits<float>::infinity();
			}
		}
	} else {
		if (collision) {
			time = -std::numeric_limits<float>::infinity();
		} else {
			time = std::numeric_limits<float>::infinity();
		}
	}
	return time;
}

// Time to collision of a ray to a line segment.
float AModelController::timeToCollision(FVector2D p, FVector2D v, FVector2D a, FVector2D b, bool collision) {
	float D = FVector2D::CrossProduct(v, b - a);

	if (D == 0) {  // ray and line are collinear
		if (collision) {
			return -std::numeric_limits<float>::infinity();;
		} else {
			return std::numeric_limits<float>::infinity();;
		}
	}

	float invD = 1.0f / D;
	float t = FVector2D::CrossProduct(a - p, b - a) * invD;
	float s = FVector2D::CrossProduct(p - a, v) * -invD;

	if (t < 0 || s < 0 || s > 1) {
		if (collision) {
			return -std::numeric_limits<float>::infinity();;
		} else {
			return std::numeric_limits<float>::infinity();;
		}
	} else {
		return t;
	}
}

/*
// X = When collision will occur (X <= 0 -> No collision)
// Y = 0, brake
// Y != 0, shift right
FVector2D AModelController::willCollide(AAgent * otherAgent) {
	// TODO: Ha Z som "Jag har förträde" istället?

	FVector2D pA = to2D(agent->GetActorLocation());			// Current location of actor
	FVector2D vA = to2D(velocity);							// Current velocity of actor
	FVector2D pB = to2D(otherAgent->GetActorLocation());	// Current location of other actor

	AModelController * bController = static_cast<AModelController *>((otherAgent->GetController()));
	FVector2D vB = to2D(bController->velocity);				// Current velocity of other actor

	FVector2D pAB = pA - pB;
	FVector2D vAB = vA - vB;

	float a = FVector2D::DotProduct(vAB, vAB);
	float b = 2 * FVector2D::DotProduct(pAB, vAB);
	float r = agent->getAgentRadius() + otherAgent->getAgentRadius();
	float c = FVector2D::DotProduct(pAB, pAB) - (r * r);

	float discriminant = (b * b) - (4 * a * c);

	FVector2D result(0, 0);
	if (discriminant > 0) {					// Possible collision
		float t0 = ((-b) - FMath::Sqrt(discriminant)) / (2 * a);
		float t1 = ((-b) + FMath::Sqrt(discriminant)) / (2 * a);

		float t = FMath::Min(t0, t1);

		if (t > 0) {						// Will collide (t < 0 -> "collision" will happen in the past)
			FVector2D cA = pA + (t * vA);
			FVector2D cB = pB + (t * vB);

			float xDiff = cA.X - cB.X;		// Distance of agents at collision
			if (FMath::Abs(xDiff) <= r) {	// If less than agentRadius, shift agents
				// TODO: Den här if-satsen lär få göras om

				result.Y = 1;
			} else {
				if (pAB.X > 0) {
					t = -1;					// Rightmost agent keeps going
				} else if (xDiff > 0) {		// If they currently have the same X-position
					t = -1;					// Rightmost agent at collision keeps going
				}
			}
		}

		result.X = t;
	} else {								// No collision
		result.X = -1;
	}

	return result;
}*/

float AModelController::getSearchDistance()
{
	return getVMax() * agent->getAgentRadius() * agentRadiusScalar;
}

float AModelController::getVMax() {
	return vMax;
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