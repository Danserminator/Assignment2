// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Agent.h"
#include "GameFramework/Pawn.h"
#include "VisibilityGraph.generated.h"

UCLASS()
class ASSIGNMENT2_API AVisibilityGraph : public APawn
{
	GENERATED_BODY()
	
private:
	TMultiMap<FVector2D, FVector2D> graph;

	TArray<TArray<FVector2D>> obstacles;

	TArray<FVector2D> vertices;

	TArray<TArray<FVector2D>> edges;

	TMap<FVector2D, FVector2D> verticesToOriginal;

	float bufferSize = 5;

public:
	UFUNCTION(BlueprintCallable, Category = "VisibilityGraph")
	void generateGraph(TArray<FVector> corners, TArray<AAgent *> agents, TArray<FVector2D> customers);

	TMultiMap<FVector2D, FVector2D> getGraph();

	TArray<FVector2D> getVertices();

	TArray<TArray<FVector2D>> getEdges();

private:
	void makeObstacles(TArray<FVector> corners);

	TArray<TArray<FVector2D>> getEdges(TArray<FVector> map);

	TArray<FVector2D> createVertices();

	FVector2D makeBufferPoint(int32 obstacle, FVector2D p0, FVector2D p1, FVector2D p2);

	bool insidePoly(TArray<FVector2D> poly, FVector2D point);

	void makeGraph(TArray<TArray<FVector2D>> edges, TArray<FVector2D> vertices);

	bool canSee(FVector2D start, FVector2D end, TArray<TArray<FVector2D>> & edges);

	bool intersect(FVector2D point1, FVector2D point2, FVector2D point3, FVector2D point4);

	void add(FVector2D start, FVector2D end);

	void writeGraphToFile(const FString fileName);
	
};
