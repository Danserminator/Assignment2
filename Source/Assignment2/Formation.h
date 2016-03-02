// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include <limits>
#include <stdexcept>
#include "Engine.h"
#include "General.h"
#include "Agent.h"
#include "Formation.generated.h"

UCLASS()
class ASSIGNMENT2_API AFormation : public APawn
{
	GENERATED_BODY()

private:
	int32 numAgents;

	FVector2D location;
	FVector2D velocity;

	TArray<AAgent *> agents;						// All agents that have found all other agents.
	TMap<AAgent *, FVector2D> agentsToLocation;		// Where every agent should be located.
	TArray<FVector2D> formationPositions;			// Position of formation relative to origin of formation

	//TArray<FVector2D> agentPositions;				// Positions of all agents that have found all other agents
	

	//TArray<FVector2D> assignedPositions;			// Which position in the formation an agent has been assigned
			// TODO: Nu använder jag bara .Y i formationPositions, göra om så assignedpositions har faktiska positionen?

	bool play;
	bool drawFormation = false;						// Wether to draw the formation positions or not
	FVector formationSize = FVector(0, 0, 20);		// Size of line representing formation positions
	FColor formationColor = FColor::Red;			// Color of line representing formation positions

public:
	// Sets default values for this pawn's properties
	AFormation();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Formation")
	void initFormation(TArray<FVector2D> positions, FVector velocity, int32 n);

	UFUNCTION(BlueprintCallable, Category = "Formation")
	void togglePlay();

	void foundAllAgents(AAgent * agent);

	FVector2D getTarget(AAgent * agent);

	FVector2D getVelocity();

private:
	void assignPositions();

	void moveFormation();

	TArray<TArray<float>> createMatrix();




	TArray<FVector2D> assignTasks(TArray<TArray<float>> & matrix);

	void stepOne(TArray<TArray<float>> & matrix);

	float getMin(TArray<float> arr);

	bool canAssign(TArray<TArray<float>> & matrix);

	void stepTwo(TArray<TArray<float>> & matrix);

	float getMinCol(TArray<TArray<float>> & matrix, int32 column);

	TArray<TArray<int32>> stepThree(TArray<TArray<float>> & matrix);

	void stepFour(TArray<TArray<float>> & matrix, TArray<TArray<int32>> & lines);

	TArray<FVector2D> assign(TArray<TArray<float>> & matrix);

	bool findSolution(TArray<TArray<float>> & matrix, TArray<TArray<bool>> & assignment, int32 row);

	float costHeuristic(FVector2D agent, FVector2D goal);	
};
