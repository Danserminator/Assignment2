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
	FVector velocity;
	float const vMax = 1;
	float const maxAngle = 18;
	bool rotating = true;


public:
	// Sets default values for this character's properties
	ADifferentialDriveController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual bool rotate();

	virtual FVector getVelocity();	

	virtual float getXVelocity(float hyp) const;

	virtual float getYVelocity(float hyp) const;
};
