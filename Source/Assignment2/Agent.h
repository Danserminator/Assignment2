// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "General.h"
#include "Agent.generated.h"

UCLASS()
class ASSIGNMENT2_API AAgent : public ACharacter {
	GENERATED_BODY()

<<<<<<< HEAD
public:
	float seeRadius;
=======
	public:
	float R;
	AFormation * formation;
>>>>>>> origin/master
	TArray<AAgent *> unseenAgents;
	TArray<AAgent *> seenAgents;

	// Sets default values for this character's properties
	AAgent();

	UFUNCTION(BlueprintCallable, Category = "Agent")
<<<<<<< HEAD
	void init(float r, TArray<AAgent *> agents);

	void findAgents();

	bool foundAllAgents();

	int32 numberUnseenAgents();

	TArray<AAgent *> getSeenAgents();

	float getSeeRadius();
=======
		void init(TArray<AAgent *> agents);

	UFUNCTION(BlueprintCallable, Category = "Agent")
		void init2(float r, AFormation * f, TArray<AAgent *> agents);

>>>>>>> origin/master
};
