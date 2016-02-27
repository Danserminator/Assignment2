// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Agent.h"

/**
 * 
 */
class ASSIGNMENT2_API VisibilityGraph
{
private:
	TMultiMap<FVector2D, FVector2D> graph;

	TArray<TArray<FVector2D>> obstacles;

	TMap<FVector2D, FVector2D> verticesToOriginal;

	float bufferSize = 0.01;

public:
	UFUNCTION(BlueprintCallable, Category = "VisibilityGraph")
	void generateGraph(TArray<FVector> * corners, TArray<AAgent *> * agents, TArray<FVector2D> * customers, VisibilityGraph & graph);

	TMultiMap<FVector2D, FVector2D> getGraph();

private:
	void makeObstacles(TArray<FVector> * corners);

	TArray<TArray<FVector2D>> getEdges(TArray<FVector> * map);

	TArray<FVector2D> getVertices();

	FVector2D makeBufferPoint(int32 obstacle, FVector2D p0, FVector2D p1, FVector2D p2);

	bool insidePoly(TArray<FVector2D> poly, FVector2D point);

	void makeGraph(TArray<TArray<FVector2D>> edges, TArray<FVector2D> vertices);

	bool canSee(FVector2D start, FVector2D end, TArray<TArray<FVector2D>> & edges);

	bool intersect(FVector2D point1, FVector2D point2, FVector2D point3, FVector2D point4);

	void add(FVector2D start, FVector2D end);
};
