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
	float const dpmcAMax = 1;
	float const dpmcVMax = 100;

	float const safetyBuffer = 0;

	float everybodyKnowsSpeed = UKismetMathLibrary::FMax(aMax * 5, vMax);

	bool moveTarget = false;
	bool firstTry = true;
	bool secondTry = true;

	float searchRadiusScalar = 1.5;		// Scalar from brake distance to obstacle search distance

public:
	ADynamicPointMassController();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual bool waypointReached() override;

	virtual FVector getAcceleration(float deltaSec) const;

	virtual float getBrakeDistance() const;

	virtual FVector2D getBrakeTarget() override;

	virtual bool updateTarget_moving();

	virtual float getSearchDistance() override;

	virtual FVector2D vSample(float deltaSec) override;
};
