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
	virtual FVector getAcceleration();

	virtual float getRotation();

	virtual float getXAcceleration(float angle);

	virtual float getYAcceleration(float angle);

	virtual FVector getVelocity();

	virtual void rotate();

	virtual float getXVelocity();

	virtual float getYVelocity();
	
};
