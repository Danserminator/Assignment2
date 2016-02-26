// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Agent.h"
#include <limits>
#include "SimulatedAnnealing.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API ASimulatedAnnealing : public AAIController
{
	GENERATED_BODY()
	
private:
	/*
	struct Node
	{
		AAgent * agent = nullptr;
		bool visited = false;
		FVector2D location;
	};

	TArray<Node> v;
	TArray<Node> r;
	*/

	TArray<FVector2D> customers;
	TArray<AAgent *> agents;

	TMap<AAgent *, TArray<FVector2D>> routes;

	bool done = false;


public:
	UFUNCTION(BlueprintCallable, Category = "Simulated Annealing")
	void simulatedAnnealing(TArray<AAgent *> agents, TArray<FVector2D> costumers);

	TMap<AAgent *, TArray<FVector2D>> getRoutes();

	UFUNCTION(BlueprintCallable, Category = "Simulated Annealing")
	TArray<FVector2D> getRoute(AAgent * agent);


private:
	TMap<AAgent *, TArray<FVector2D>> annealing();

	TMap<AAgent *, TArray<FVector2D>> initialConfiguration();

	TMap<AAgent *, TArray<FVector2D>> neighbour(TMap<AAgent *, TArray<FVector2D>> routes);
	
	TMap<AAgent *, TArray<FVector2D>> move(TMap<AAgent *, TArray<FVector2D>> routes);

	TArray<FVector2D> addToRoute(AAgent * agent, TArray<FVector2D> route, FVector2D Costumer);

	TMap<AAgent *, TArray<FVector2D>> replaceHighestAverage(TMap<AAgent *, TArray<FVector2D>> routes);

	TMap<AAgent *, TArray<FVector2D>> insertBest(TMap<AAgent *, TArray<FVector2D>> routes, TArray<FVector2D> costumers, TArray<AAgent *> agents);
	
	float ifAddedCost(TArray<FVector2D> route, FVector2D costumer);

	TArray<FVector2D> addToRoute(TArray<FVector2D> route, FVector2D costumer);
	
	float cost(FVector2D start, FVector2D stop) const;

	float cost(TArray<FVector2D> route) const;

	float cost(TMap<AAgent *, TArray<FVector2D>> routes) const;

	FVector2D to2D(FVector vector) const;
};
