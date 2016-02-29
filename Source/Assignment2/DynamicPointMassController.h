// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelController.h"
#include "Engine.h"
#include "Agent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DynamicPointMassController.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API ADynamicPointMassController : public AModelController
{
	GENERATED_BODY()

private:
	FVector acceleration;
	float const mass = 1;
	float const aMax = 1;
	float const vMax = 100;

	float const safetyBuffer = 0.1;


public:
	// Sets default values for this character's properties
	ADynamicPointMassController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual FVector2D getBrakeTarget() override;

	virtual bool waypointReached() override;

	virtual FVector getAcceleration() const;
};
