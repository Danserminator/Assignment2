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
	
private:
	FVector velocity;
	float const vMax = 10;
	float const maxAngle = 18;
	float const L = 1;
	bool rotating = true;


public:
	// Sets default values for this character's properties
	AKinematicCarController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual void rotate();

	virtual FVector getVelocity();

	virtual float getXVelocity(float hyp) const;

	virtual float getYVelocity(float hyp) const;
};
