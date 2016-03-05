// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelController.h"
#include "Engine.h"
#include "Agent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DifferentialDriveController.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API ADifferentialDriveController : public AModelController
{
	GENERATED_BODY()
	
private:
	float const ddcVMax = 10;
	float const maxAngle = 18;
	bool rotating = true;

	bool first = true;

public:
	ADifferentialDriveController();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual bool rotate(float deltaSec);

	virtual FVector getVelocity(float deltaSec);

	virtual FVector getBestVelocity(float deltaSec);

	virtual float getXVelocity(float deltaSec) const;

	virtual float getYVelocity(float deltaSec) const;

	virtual FVector2D vSample(float deltaSec) override;
};
