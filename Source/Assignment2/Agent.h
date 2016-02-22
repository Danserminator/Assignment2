// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Agent.generated.h"

UCLASS()
class ASSIGNMENT2_API AAgent : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stationary Formation")
	FVector2D target;

	// Sets default values for this character's properties
	AAgent();
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void setTarget(FVector2D t);
	
};
