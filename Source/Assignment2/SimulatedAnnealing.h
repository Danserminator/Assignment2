// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Agent.h"
#include <limits>
#include <ctime>
#include "Settings.h"
#include "VisibilityGraph.h"
#include "AStar.h"
#include "General.h"
#include "SimulatedAnnealing.generated.h"

/**
*
*/
UCLASS()
class ASSIGNMENT2_API ASimulatedAnnealing : public APawn
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

	float temperature = 5000;
	float alpha = 0.85;	// Temperature reduction multiplier
	float beta = 1.05;	// Iteration multiplier
	float M0 = 5;		// Time until next parameter update
	float maxTime = 20;

	TArray<FVector2D> customers;
	TArray<AAgent *> agents;
	AVisibilityGraph * graph;

	TMap<AAgent *, TArray<FVector2D>> routes;

	bool done = false;

	int32 numberToChange = 3;


public:
	UFUNCTION(BlueprintCallable, Category = "Simulated Annealing")
		void simulatedAnnealing(AVisibilityGraph * graph, TArray<AAgent *> agents, TArray<FVector2D> inputCustomers,
			float temperature, float alpha, float beta, float M0, float maxTime);

	TMap<AAgent *, TArray<FVector2D>> getRoutes();

	UFUNCTION(BlueprintCallable, Category = "Simulated Annealing")
		TArray<FVector2D> getRoute(AAgent * agent);


private:
	TMap<AAgent *, TArray<FVector2D>> annealing();

	TMap<AAgent *, TArray<FVector2D>> initialConfiguration();

	TMap<AAgent *, TArray<FVector2D>> neighbour(TMap<AAgent *, TArray<FVector2D>> routes);

	TMap<AAgent *, TArray<FVector2D>> move(TMap<AAgent *, TArray<FVector2D>> routes);

	TArray<FVector2D> addToRoute(AAgent * agent, TArray<FVector2D> route, FVector2D customer);

	TMap<AAgent *, TArray<FVector2D>> replaceHighestAverage(TMap<AAgent *, TArray<FVector2D>> routes);

	TMap<AAgent *, TArray<FVector2D>> insertBest(TMap<AAgent *, TArray<FVector2D>> routes, TArray<FVector2D> customers, TArray<AAgent *> agents);

	float ifAddedCost(TArray<FVector2D> route, FVector2D customer);

	TArray<FVector2D> addToRoute(TArray<FVector2D> route, FVector2D customer);

	float cost(FVector2D start, FVector2D stop) const;

	float cost(TArray<FVector2D> route) const;

	float cost(AAgent * agent, TArray<FVector2D> route) const;

	float cost(TMap<AAgent *, TArray<FVector2D>> routes) const;

	void writeRoutesToFile(const FString fileName);
};
