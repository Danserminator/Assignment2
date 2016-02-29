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

	AAgent * agent;
	AFormation * formation;
	TArray<AAgent *> seenAgents, unseenAgents;

	TArray<FVector2D> waypoints;
	bool followPath = false;
	int32 waypointsIndex = 0;

	float R;
	FVector2D target;
	int32 formationPosition;
	bool movingFormation = false;

	FVector velocity;

	float const errorTolerance = 5;
	
	FColor velocityColor = FColor::Blue;		// Color of line representing velocity
	FColor accelerationColor = FColor::Yellow;	// Color of line representing acceleration
	FColor searchColor = FColor::Magenta;		// Color of line representing target when searching for more agents
	FColor radiusColor = FColor(0, 200, 0);		// Color of the circle representing radius

	bool searching = false;						// Whether the agent is searching for nearby actors or not
	bool stopped = true;						// Agent has stopped moving after finding all agents
	float searchSize = 10;						// Size of point representing target when searching for more agents
	float radiusSegments = 24;					// # of segments in the circle representing radius

public:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category = "Model Controller")
	virtual void setWaypoints(AVisibilityGraph * graph, TArray<FVector2D> customers);

	FVector2D getClosest(TArray<FVector2D> positions, FVector2D position);

protected:
	virtual bool setTarget();

	virtual void findNewAgents();

	virtual FVector2D approachAgents();

	virtual void adjustTarget(FVector2D formationVelocity);

	virtual FVector2D getBrakeTarget();

	virtual bool waypointReached();

	virtual FRotator getRotation(FVector start, FVector2D target) const {
		return getRotation(start, FVector(target, 0));
	}

	virtual FRotator getRotation(FVector start, FVector target) const;

	virtual void setRotation();

	virtual void drawLine(FVector line, FColor color) const;

	virtual FVector2D to2D(FVector vector) const;

	virtual FVector to3D(FVector2D vector) const;

	void writeWaypointsToFile(const FString fileName);
};
