// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "VisibilityGraph.h"

#define OUTPUT

void AVisibilityGraph::generateGraph(TArray<FVector> corners, TArray<AAgent*> agents, TArray<FVector2D> customers)
{
	makeObstacles(corners);

	TArray<TArray<FVector2D>> edges = getEdges(corners);

	vertices = createVertices();

	// Add actor locations to visibility graph
	FVector aLoc;
	for (int32 c = 0; c < agents.Num(); c++) {
		aLoc = agents[c]->GetActorLocation();
		vertices.Add(FVector2D(aLoc.X, aLoc.Y));
	}

	// Add customer locations to visibility graph
	for (int32 c = 0; c < customers.Num(); c++) {
		vertices.Add(customers[c]);
	}

	makeGraph(edges, vertices);

#ifdef OUTPUT
	writeGraphToFile("VisibilityGraph.txt");
#endif
}

TMultiMap<FVector2D, FVector2D> AVisibilityGraph::getGraph()
{
	return graph;
}

TArray<FVector2D> AVisibilityGraph::getVertices()
{
	return vertices;
}

void AVisibilityGraph::makeObstacles(TArray<FVector> corners)
{
	obstacles = TArray<TArray<FVector2D>>();

	FVector2D vertice;
	bool obstDone = false;
	TArray<FVector2D> obstacle;
	for (int32 c = 0; c < corners.Num(); c++) {
		if (obstDone) {
			obstDone = false;
			obstacles.Add(obstacle);
			obstacle.Empty();
		}

		vertice = FVector2D(corners[c].X, corners[c].Y);

		obstacle.Add(vertice);

		if (corners[c].Z == 3) {
			obstDone = true;
		}
	}

	if (obstDone) {
		obstacles.Add(obstacle);
	}
}

TArray<TArray<FVector2D>> AVisibilityGraph::getEdges(TArray<FVector> map)
{
	TArray<TArray<FVector2D>> edges;

	FVector2D init;
	FVector2D startLine;
	FVector2D endLine;
	bool newObst = true;
	for (int32 c = 0; c < map.Num(); c++) {
		if (newObst) {
			newObst = false;
			init = FVector2D(map[c].X, map[c].Y);
			startLine = FVector2D(map[c].X, map[c].Y);
			endLine = FVector2D(map[c + 1].X, map[c + 1].Y);
		} else {
			startLine = endLine;
			if (map[c].Z == 1) {
				endLine = FVector2D(map[c + 1].X, map[c + 1].Y);
			} else {
				newObst = true;
				endLine = init;
			}
		}

		TArray<FVector2D> temp;
		temp.Add(startLine);
		temp.Add(endLine);
		edges.Add(temp);
	}

	return edges;
}

TArray<FVector2D> AVisibilityGraph::createVertices()
{
	vertices = TArray<FVector2D>();

	FVector2D currPoint, lastPoint, nextPoint;
	for (int32 c = 0; c < obstacles.Num(); c++) {
		for (int32 g = 0; g < obstacles[c].Num(); g++) {
			if (g == 0) {
				lastPoint = obstacles[c][obstacles[c].Num() - 1];
			} else {
				lastPoint = obstacles[c][g - 1];
			}

			currPoint = obstacles[c][g];

			if (g + 1 == obstacles[c].Num()) {
				nextPoint = obstacles[c][0];
			} else {
				nextPoint = obstacles[c][g + 1];
			}

			FVector2D vertice = makeBufferPoint(c, lastPoint, currPoint, nextPoint);
			vertices.Add(vertice);
			verticesToOriginal.Add(vertice, currPoint);
		}
	}

	return vertices;
}

FVector2D AVisibilityGraph::makeBufferPoint(int32 obstacle, FVector2D p0, FVector2D p1, FVector2D p2)
{
	FVector2D line1 = p1 - p0;
	FVector2D line2 = p1 - p2;

	line1.Normalize();
	line2.Normalize();

	FVector2D result = line1 + line2;
	result.Normalize();

	FVector2D bufferPoint = p1 + bufferSize * result;

	if (insidePoly(obstacles[obstacle], bufferPoint)) {
		return p1 - (bufferSize * result);
	} else {
		return bufferPoint;
	}
}

bool AVisibilityGraph::insidePoly(TArray<FVector2D> poly, FVector2D point)
{
	bool insidePoly = false;
	for (int32 c = 0, g = poly.Num() - 1; c < poly.Num(); g = c++) {
		if (((poly[c].Y > point.Y) != (poly[g].Y > point.Y)) &&
			(point.X < (poly[g].X - poly[c].X) * (point.Y - poly[c].Y) / (poly[g].Y - poly[c].Y) + poly[c].X)) {
			insidePoly = !insidePoly;
		}
	}

	return insidePoly;
}

void AVisibilityGraph::makeGraph(TArray<TArray<FVector2D>> edges, TArray<FVector2D> vertices) {
	for (int32 c = 0; c < vertices.Num(); c++) {
		for (int32 g = 0; g < vertices.Num(); g++) {
			if (c == g) continue;
			if (canSee(vertices[c], vertices[g], edges)) {
				add(vertices[c], vertices[g]);
				//add(vertices[g], vertices[c]);
			}
		}
	}

	#ifdef OUTPUT1
	TArray<FVector2D> keys;
	graph.GetKeys(keys);

	UE_LOG(LogTemp, Warning, TEXT("Visibility graph:"));
	for (int32 c = 0; c < keys.Num(); c++) {
		TArray<FVector2D> connected;
		graph.MultiFind(keys[c], connected);

		FString str;
		for (int32 g = 0; g < connected.Num(); g++) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(connected[g].X));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(connected[g].Y));
			str.Append("), ");
		}

		UE_LOG(LogTemp, Warning, TEXT("Key #%d (%f, %f): %s\r\n"), c + 1, keys[c].X, keys[c].Y, *str);
	}
	#endif
}

bool AVisibilityGraph::canSee(FVector2D start, FVector2D end, TArray<TArray<FVector2D>> & edges)
{
	/*
	for (int32 c = 0; c < edges.Num(); c++) {
	if (intersect(start, end, edges[c][0], edges[c][1])) {
	return false;
	}
	}
	*/

	for (int32 c = 0; c < edges.Num(); c++) {
		if (intersect(start, end, edges[c][0], edges[c][1])) {
			return false;
		}
	}

	for (int32 c = 0; c < edges.Num(); c++) {
		if (verticesToOriginal.Contains(start)) {
			if (edges[c].Contains(verticesToOriginal[start])) {
				continue;
			}
		}
		if (verticesToOriginal.Contains(end)) {
			if (edges[c].Contains(verticesToOriginal[end])) {
				continue;
			}
		}

		if (intersect(start, end, edges[c][0] - bufferSize, edges[c][1] + bufferSize) || intersect(start, end, edges[c][0] + bufferSize, edges[c][1] - bufferSize)) {
			return false;
		}
	}

	return true;
}

bool AVisibilityGraph::intersect(FVector2D point1, FVector2D point2, FVector2D point3, FVector2D point4)
{
	FVector2D direction1 = point2 - point1;
	FVector2D direction2 = point4 - point3;

	// For sanity
	if (-direction2.X * direction1.Y + direction1.X * direction2.Y == 0) {
		return false;
	}

	float s = (-direction1.Y * (point1.X - point3.X) + direction1.X * (point1.Y - point3.Y)) / (-direction2.X * direction1.Y + direction1.X * direction2.Y);
	float t = (direction2.X * (point1.Y - point3.Y) - direction2.Y * (point1.X - point3.X)) / (-direction2.X * direction1.Y + direction1.X * direction2.Y);

	// For John
	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		return true;
	}

	return false;
}

void AVisibilityGraph::add(FVector2D start, FVector2D end)
{
	graph.Add(start, end);
}

void AVisibilityGraph::writeGraphToFile(const FString fileName)
{
	FString str;
	for (int32 i = 0; i < vertices.Num(); i++) {
		TArray<FVector2D> stuff;
		graph.MultiFind(vertices[i], stuff);
		for (int32 c = 0; c < stuff.Num() - 1; c++) {
			str.Append(FString::SanitizeFloat(vertices[i].Y));
			str.Append("\t");
			str.Append(FString::SanitizeFloat(vertices[i].X));

			str.Append("\t\t");

			str.Append(FString::SanitizeFloat(stuff[c].Y));
			str.Append("\t");
			str.Append(FString::SanitizeFloat(stuff[c].X));
			str.Append("\r\n");
		}
		if (stuff.Num() != 0) {
			str.Append(FString::SanitizeFloat(vertices[i].Y));
			str.Append("\t");
			str.Append(FString::SanitizeFloat(vertices[i].X));

			str.Append("\t\t");

			
			str.Append(FString::SanitizeFloat(stuff[stuff.Num() - 1].Y));
			str.Append("\t");
			str.Append(FString::SanitizeFloat(stuff[stuff.Num() - 1].X));
			str.Append("\r\n");
		}
		
	}

	FString projectDir = FPaths::GameDir();
	projectDir += "Output Data/" + fileName;
	FFileHelper::SaveStringToFile(str, *projectDir);
}