// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "General.h"
#include "Agent.generated.h"

UCLASS()
class ASSIGNMENT2_API AAgent : public ACharacter {
	GENERATED_BODY()

public:
	int32 id;
	float seeRadius, agentRadius;
	TArray<AAgent *> unseenAgents;
	TArray<AAgent *> seenAgents;

	// Sets default values for this character's properties
	AAgent();

	UFUNCTION(BlueprintCallable, Category = "Agent")
	void init(TArray<AAgent *> agents, float r = 0, int32 id = 0);

	void findAgents();

	bool foundAllAgents();

	int32 numberUnseenAgents();

	TArray<AAgent *> getSeenAgents();

	float getSeeRadius();

	UFUNCTION(BlueprintCallable, Category = "Agent")
	void setAgentRadius(float agentRadius);

	float getAgentRadius();

	UFUNCTION(BlueprintCallable, Category = "Agent")
	int32 getAgentID();
};
