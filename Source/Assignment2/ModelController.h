// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Engine.h"
#include "Agent.h"
#include "Formation.h"
#include "Kismet/KismetMathLibrary.h"
#include "AStar.h"
#include "VisibilityGraph.h"
#include <limits>
#include "General.h"
#include "ModelController.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API AModelController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AgentPlay)
	bool play;
	
protected:
	float totalTime;
	AAgent * agent;
	AFormation * formation;
	AVisibilityGraph * graph;

	TArray<FVector2D> waypoints;
	TArray<FVector2D> customers;
	//bool followPath = false;
	int32 waypointsIndex = 0;
	int32 customersIndex = 0;

	FVector2D target;
	
	//int32 formationPosition;
	//bool movingFormation = false;

	FVector velocity;
	float vMax = 0;
	float aMax = 0;								// TODO: Kör (2 * vMax) tills vidare

	float errorTolerance = 3;					// Change this one in every model FFS!!!
	
	FColor velocityColor = FColor::Blue;		// Color of line representing velocity
	FColor accelerationColor = FColor::Yellow;	// Color of line representing acceleration
	FColor searchColor = FColor::Magenta;		// Color of line representing target when searching for more agents
	FColor radiusColor = FColor(0, 200, 0);		// Color of the circle representing radius
	FColor collisionColor = FColor::Red;		// Color of the line representing agent collision

	FVector collisionSize = FVector(0, 0, 20);	// Shape of line that is drawn when agents collide

	bool followPath = false;
	bool movingFormation = false;
	bool avoidAgents = false;
	bool everybodyKnows = false;

	bool searching = false;							// Whether the agent is searching for nearby actors or not
	bool stopped = true;							// Agent has stopped moving after finding all agents
	float searchSize = 10;							// Size of point representing target when searching for more agents
	float radiusSegments = 24;						// # of segments in the circle representing radius

	float agentSearchScalar = 4;					// Scalar from agent radius to obstacle search distance

	bool collided = false;							// If agent has collided
	int32 vSamples = 250;							// Number of velocity samples
	TArray<TTuple<float, int32, int32>> neighbours;	// Neighbours of agent, in increasing distance
		const int32 AGENT = 0;						// float that represents that neighbour is an agent
		const int32 OBSTACLE = 1;					// float that represents that neighbour is an obstacle
	float safetyFactor = 10;						// Higher safety = less 'agressive'
	float agentRadiusScalar = 2;					// Scalar for agent radius, so agent is bigger when checking for collisions

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category = "Model Controller")
	virtual void setWaypoints(AVisibilityGraph * graph, TArray<FVector2D> customers);

	UFUNCTION(BlueprintCallable, Category = "Model Controller")
	virtual void setGoal(FVector2D goal);

	UFUNCTION(BlueprintCallable, Category = "Model Controller")
	void setParameters(AFormation * formation, bool followPath, bool movingFormation, bool avoidObstacles);

	FVector2D getClosest(TArray<FVector2D> positions, FVector2D position);

protected:
	virtual bool updateTarget();

	virtual bool updateTarget_path();

	virtual bool updateTarget_moving();

	virtual bool updateTarget_still();

	virtual bool isMovingTowardsTarget(FVector2D target);

	virtual FVector2D approachAgents();

	virtual FVector2D getBrakeTarget();

	virtual bool waypointReached();

	virtual float getRotation(FVector start, FVector2D target) const {
		return getRotation(start, to3D(target));
	}

	virtual float getRotation(FVector start, FVector target) const;

	virtual void setRotation();

	virtual void drawLine(FVector line, FColor color) const;

	void writeWaypointsToFile(const FString fileName);

	void updateNeighbours();

	void computeObstacleNeighbours();

	void computeAgentNeighbours();

	void adjustVelocity(FVector2D vPref, float deltaSec);

	virtual FVector2D vSample(float deltaSec);

	// Time to collision of a ray to a disc
	float timeToCollision(FVector2D p, FVector2D v, FVector2D p2, float radius, bool collision);

	// Time to collision of a ray to a line segment.
	float timeToCollision(FVector2D p, FVector2D v, FVector2D a, FVector2D b, bool collision);

	//FVector2D willCollide(AAgent * otherAgent);

	virtual float getSearchDistance();

	virtual float getVMax();
};
