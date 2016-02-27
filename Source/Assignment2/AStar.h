// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <limits>

/**
 * 
 */
class ASSIGNMENT2_API AStar
{
public:
	static TArray<FVector2D> getPath(TMultiMap<FVector2D, FVector2D> graph, TArray<FVector2D> vertices, FVector2D start, FVector2D goal);

private:
	static int32 findLowestFScore(TArray<FVector2D> openSet, TMap<FVector2D, float> fScore);

	static TMap<FVector2D, float> infMap(TArray<FVector2D> vertices);

	static float dist_between(FVector2D start, FVector2D goal);

	static float heuristic_cost_estimate(FVector2D start, FVector2D goal);

	static TArray<FVector2D> reconstruct_path(TMap<FVector2D, FVector2D> cameFrom, FVector2D current);

	static void getNeighbours(TMultiMap<FVector2D, FVector2D> graph, TQueue<FVector2D> & queue, FVector2D current);
};
