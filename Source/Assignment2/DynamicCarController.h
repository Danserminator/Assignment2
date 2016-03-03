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
	float const maxAngle = 18;
	float const L = 1;
	float const safetyBuffer = 0.1;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual FVector getAcceleration(float deltaSec);

	virtual float rotateVelocity(float deltaSec);

	virtual float getXAcceleration(float angle, float deltaSec);

	virtual float getYAcceleration(float angle, float deltaSec);

	virtual FVector getVelocity(float deltaSec);

	virtual float rotate(float deltaSec);

	virtual float getXVelocity(float deltaSec);

	virtual float getYVelocity(float deltaSec);
	
};
