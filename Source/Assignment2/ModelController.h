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


public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void setTarget();

	virtual void findNewAgents();

	virtual FVector2D approachAgents();

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
