// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelController.h"
#include "DubinsPath.h"
#include "KinematicCarController.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API AKinematicCarController : public AModelController
{
	GENERATED_BODY()
	
protected:
	float const kccVMax = 10;
	float const maxAngle = 57.2957795;
	float const L = 10;

	bool first = true;

public:
	AKinematicCarController();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual float rotate(float deltaSec) const;

	virtual FVector getVelocity(float deltaSec);

	virtual float getXVelocity(float hyp) const;

	virtual float getYVelocity(float hyp) const;

	virtual FVector2D vSample(float deltaSec) override;
};
