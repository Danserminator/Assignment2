// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelController.h"
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
	float const aMax = 1;
	float const vMax = 10;
	float const maxAngle = 57.2957795;
	float const L = 1;
	float const safetyBuffer = 0.1;

	float v = 0;

	bool first = true;

	float searchRadiusScalar = 1.5;		// Scalar from brake distance to obstacle search distance

public:
	ADynamicCarController();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual bool waypointReached() override;

	virtual float getAcceleration(float deltaSec) const;

	virtual float rotate(float deltaSec) const;

	virtual float getBrakeDistance() const;

	virtual float getSearchDistance() override;
};
