// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Engine.h"
#include "Agent.h"
#include "Formation.h"
#include "Kismet/KismetMathLibrary.h"
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

	float R;
	FVector2D target;
	int32 formationPosition;

	FVector velocity;

	float const errorTolerance = 1;
	
	FColor velocityColor = FColor::Blue;		// Color of line representing velocity
	FColor accelerationColor = FColor::Yellow;	// Color of line representing acceleration
	FColor searchColor = FColor::Magenta;		// Color of line representing target when searching for more agents
	FColor radiusColor = FColor(0, 200, 0);		// Color of the circle representing radius

	bool searching = false;						// Whether the agent is searching for nearby actors or not
	bool stopped = true;						// Agent has stopped moving after finding all agents
	float searchSize = 10;						// Size of point representing target when searching for more agents
	float radiusSegments = 24;					// # of segments in the circle representing radius

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void setTarget();

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
};
