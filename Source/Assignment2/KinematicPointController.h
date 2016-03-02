// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelController.h"
#include "Engine.h"
#include "Agent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KinematicPointController.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API AKinematicPointController : public AModelController
{
	GENERATED_BODY()
	
private:
	FVector velocity;
	float const vMax = 100;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
private:
	virtual FVector getVelocity() const;
};
