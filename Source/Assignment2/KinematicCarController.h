// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelController.h"
#include "KinematicCarController.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API AKinematicCarController : public AModelController
{
	GENERATED_BODY()
	
protected:
	float const vMax = 10;
	float const maxAngle = 18;
	float const L = 1;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void rotate();

	virtual FVector getVelocity();

	virtual float getXVelocity(float hyp) const;

	virtual float getYVelocity(float hyp) const;
};
