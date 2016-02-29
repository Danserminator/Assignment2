// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Formation.h"
#include "Agent.generated.h"

UCLASS()
class ASSIGNMENT2_API AAgent : public ACharacter
{
	GENERATED_BODY()

public:
	float R;
	AFormation * formation;
	TArray<AAgent *> unseenAgents;

	// Sets default values for this character's properties
	AAgent();

	UFUNCTION(BlueprintCallable, Category = "Agent")
	void init(float r, AFormation * f, TArray<AAgent *> agents);
	
};
