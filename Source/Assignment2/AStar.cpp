// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "AStar.h"

#define OUTPUT

TArray<FVector2D> AStar::getPath(TMultiMap<FVector2D, FVector2D> graph, TArray<FVector2D> vertices, FVector2D start, FVector2D goal)
{
	TArray<FVector2D> closedSet;	// The set of nodes already evaluated.
	TArray<FVector2D> openSet;		// The set of tentative nodes to be evaluated, initially containing the start node
	openSet.Add(start);

	TMap<FVector2D, FVector2D> cameFrom;	// The map of navigated nodes.

	TMap<FVector2D, float> gScore = infMap(vertices);	// map with default value of Infinity

	gScore[start] = 0;	// Cost from start along best known path.
						// Estimated total cost from start to goal through y.

	TMap<FVector2D, float> fScore = infMap(vertices);	// map with default value of Infinity

	fScore[start] = heuristic_cost_estimate(start, goal);

	while (openSet.Num() > 0) {
		int32 index = findLowestFScore(openSet, fScore);
		FVector2D current = openSet[index]; // openSet.Pop();	// the node in OpenSet having the lowest f_score[] value
		if (current == goal) {
			TArray<FVector2D> path = reconstruct_path(cameFrom, goal);
			return path;
		}

		openSet.RemoveAt(index);
		closedSet.Add(current);

		TQueue<FVector2D> neighbours;
		getNeighbours(graph, neighbours, current);

		FVector2D neighbour;
		while (neighbours.Dequeue(neighbour)) {
			if (closedSet.Contains(neighbour)) {
				continue;	// Ignore the neighbor which is already evaluated.
			}

			float tentative_g_score = gScore[current] + dist_between(current, neighbour); // length of this path.

			if (!openSet.Contains(neighbour)) {
				openSet.Add(neighbour);
			} else if (tentative_g_score >= gScore[neighbour]) {
				continue;		// This is not a better path.
			}

			// This path is the best until now. Record it!
			cameFrom.Add(neighbour, current);

			gScore[neighbour] = tentative_g_score;
			fScore[neighbour] = gScore[neighbour] + heuristic_cost_estimate(neighbour, goal);
		}
	}

	throw std::exception("No path found!");
}

int32 AStar::findLowestFScore(TArray<FVector2D> openSet, TMap<FVector2D, float> fScore)
{
	int32 index = 0;
	float score = std::numeric_limits<float>::infinity();
	for (int32 c = 0; c < openSet.Num(); c++) {
		if (fScore[openSet[c]] < score) {
			index = c;
			score = fScore[openSet[c]];
		}
	}

	return index;
}

TMap<FVector2D, float> AStar::infMap(TArray<FVector2D> vertices)
{
	TMap<FVector2D, float> map;
	for (int32 c = 0; c < vertices.Num(); c++) {
		map.Add(vertices[c], std::numeric_limits<float>::infinity());
	}

	return map;
}

float AStar::dist_between(FVector2D start, FVector2D goal)
{
	return FVector2D::Distance(start, goal);
}

float AStar::heuristic_cost_estimate(FVector2D start, FVector2D goal)
{
	return dist_between(start, goal);
}

TArray<FVector2D> AStar::reconstruct_path(TMap<FVector2D, FVector2D> cameFrom, FVector2D current)
{
	TArray<FVector2D> totalPath;

	totalPath.Add(current);
	while (cameFrom.Contains(current)) {
		current = cameFrom[current];
		totalPath.Insert(current, 0);
	}

	return totalPath;
}

void AStar::getNeighbours(TMultiMap<FVector2D, FVector2D> graph, TQueue<FVector2D> & queue, FVector2D current)
{
	TArray<FVector2D> neighbours;
	graph.MultiFind(current, neighbours);

	for (int32 c = 0; c < neighbours.Num(); c++) {
		queue.Enqueue(neighbours[c]);
	}
}