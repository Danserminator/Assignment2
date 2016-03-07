// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelController.h"
#include "DubinsPath.h"
#include "DynamicCarController.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API ADynamicCarController : public AModelController
{
	GENERATED_BODY()

private:
	FVector acceleration;
	float const dccAMax = 1;
	float const dccVMax = 10;
	float const maxAngle = 45;
	float const L = 10;
	float const safetyBuffer = 1;

	float v = 0;

	bool first = true;

	float searchRadiusScalar = 1.5;		// Scalar from brake distance to obstacle search distance

	bool moveTarget = false;
	bool firstTry = true;
	bool secondTry = true;

	bool backing = false;

public:
	ADynamicCarController();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	bool lookingAtTarget();

	virtual float getAcceleration(float deltaSec) const;

	virtual float rotate(float deltaSec) const;

	virtual float getBrakeDistance() const;

	virtual float getSearchDistance() override;

	virtual bool updateTarget_moving() override;

	virtual void simulate();

	virtual FVector2D vSample(float deltaSec) override;
};
