// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "Agent.h"
#include "Block.h"
#include "Customer.h"
#include "Formation.h"
#include <iostream>
#include "Settings.h"
#include "MapGenerator.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API AMapGenerator : public ALevelScriptActor
{
	GENERATED_BODY()

	private:
	const FString inputFolder = "Input Data/";					// Location of all input files
	const FString obstacleFile = "obstacles.txt";				// Name of file that contains obstacles
	const FString positionsFile = "positions.txt";				// Name of file that contains agent positions
	const FString customersFile = "customers.txt";				// Name of file that contains customer positions

	const float gridSize = 1;									// Scalar value for generated map

	public:
	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void generateObstacles();

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void generateAgents(float r, AFormation * formation, TArray<AAgent *> & agents);

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void generateFormation(float d, TArray<FVector2D> & positions);

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void generateCustomers(TArray<FVector2D> & customers);
	
	private:
	TArray<TArray<float>> readData(const FString fileName);

	TArray<TArray<FVector2D>> getEdges(TArray<TArray<float>> map);

	FVector2D gridToLocation(float X, float Y);

	FVector2D gridToLocation(FVector2D vector);
};
