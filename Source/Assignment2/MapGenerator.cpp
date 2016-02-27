// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "MapGenerator.h"

//#define OUTPUT

void AMapGenerator::generateObstacles(TArray<FVector> & corners)
{
	// Get MazeBlock blueprint
	auto cls = StaticLoadObject(UObject::StaticClass(), nullptr,
								TEXT("Blueprint'/Game/TopDownCPP/Blueprints/BLock.Block'"));
	UBlueprint * bp = Cast<UBlueprint>(cls);
	TSubclassOf<class UObject> blockBP = (UClass*)bp->GeneratedClass;

	// Read obstacles file and generate edges
	TArray<TArray<float>> obstacleCorners = readData(obstacleFile);
	edges = getEdges(obstacleCorners);

	// Group vertices into obstacles
	FVector vertice;
	for (int32 c = 0; c < obstacleCorners.Num(); c++) {
		vertice = FVector(gridToLocation(obstacleCorners[c][0], obstacleCorners[c][1]), obstacleCorners[c][2]);
		corners.Add(vertice);
	}

	// Generate blocks with edges
	for (int32 c = 0; c < edges.Num(); c++) {
		FVector2D edgeStart = gridToLocation(edges[c][0]);
		FVector2D edgeEnd = gridToLocation(edges[c][1]);

		FVector2D line = edgeEnd - edgeStart;

		FRotator rot = FVector(line.X, line.Y, 0).Rotation();

		float distance = FVector2D::Distance(edgeStart, edgeEnd);

		FVector2D location(edgeStart + 0.5 * line);
		ABlock * block = GWorld->GetWorld()->SpawnActor<ABlock>(blockBP, FVector(location, 0), rot);
		block->changeScale(distance / gridSize);
	}
}

void AMapGenerator::generateAgents(float r, AFormation * formation, TArray<AAgent *> & agents)
{
	// Get Agent blueprint
	auto cls = StaticLoadObject(UObject::StaticClass(), nullptr,
						   TEXT("Blueprint'/Game/TopDownCPP/Blueprints/Agent.Agent'"));
	UBlueprint * bp = Cast<UBlueprint>(cls);
	TSubclassOf<class UObject> agentBP = (UClass*)bp->GeneratedClass;

	// Read positions of agents
	TArray<TArray<float>> locations = readData(positionsFile);

	formation->setNumAgents(10);

	// Spawn agents
	for (int32 c = 0; c < locations.Num(); c++) {
		FVector location = FVector(gridToLocation(locations[c][0], locations[c][1]), 0);
		AAgent * agent = GWorld->GetWorld()->SpawnActor<AAgent>(agentBP, location, { 0,0,0 });

		agentsPositions.Add(FVector2D(locations[c][0], locations[c][1]));

		agents.Add(agent);

		#ifdef OUTPUT
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("Agent #%d: {%f, %f}\r\n"), c, agents[c]->GetActorLocation().X, agents[c]->GetActorLocation().Y));
		#endif
	}

	for (int32 c = 0; c < locations.Num(); c++) {
		agents[c]->init(r, formation, agents);
	}
}

void AMapGenerator::generateFormation(float d, TArray<FVector2D> & positions)
{
	int32 num = 10;										// TODO: FIX?
	int32 yLoc[] = { 0,1,2,3,0,1,2,3,1,2 };				// Y-locations for formation
	int32 xLoc[] = { 0,0,0,0,1,1,1,1,2,2 };				// X-locations for formation

	int32 yMax = 0, xMax = 0;
	for (int32 c = 0; c < num; c++) {
		if (yMax < yLoc[c]) yMax = yLoc[c];
		if (xMax < xLoc[c]) xMax = xLoc[c];
	}

	for (int32 c = 0; c < num; c++) {
		positions.Add(gridToLocation((xLoc[c] - (xMax / 2)) * d, (yLoc[c] - (yMax / 2)) * d));
		#ifdef OUTPUT
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Blue, FString::Printf(TEXT("Position #%d: {%f, %f}\r\n"), c, positions[c].X, positions[c].Y));
		#endif
	}
}

void AMapGenerator::generateCustomers(TArray<FVector2D> & customers)
{
	// Get Agent blueprint
	auto cls = StaticLoadObject(UObject::StaticClass(), nullptr,
		TEXT("Blueprint'/Game/TopDownCPP/Blueprints/Customer.Customer'"));
	UBlueprint * bp = Cast<UBlueprint>(cls);
	TSubclassOf<class UObject> customerBP = (UClass*)bp->GeneratedClass;

	// Read positions of customers
	TArray<TArray<float>> locations = readData(customersFile);

	// Spawn customers
	for (int32 c = 0; c < locations.Num(); c++) {
		FVector location = FVector(gridToLocation(locations[c][0], locations[c][1]), 0);

		GWorld->GetWorld()->SpawnActor<ACustomer>(customerBP, location, FRotator(0, 0, 0));

		customers.Add(FVector2D(locations[c][0], locations[c][1]));

		customersPositions.Add(FVector2D(locations[c][0], locations[c][1]));

#ifdef OUTPUT
		//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("Agent #%d: {%f, %f}\r\n"), c, agents[c]->GetActorLocation().X, agents[c]->GetActorLocation().Y));
#endif
	}
}

TArray<TArray<float>> AMapGenerator::readData(const FString fileName)
{
	TArray<FString> strArray;
	FString projectDir = FPaths::GameDir();
	projectDir += inputFolder + fileName;
	FFileHelper::LoadANSITextFileToStrings(*projectDir, NULL, strArray);

	TArray<TArray<float>> data;
	for (FString line : strArray) {
		if (line.Trim().IsEmpty()) continue;

		TArray<float> row;

		FString solid;

		while (line.Split(FString("\t"), &solid, &line)) {
			row.Add(FCString::Atof(*solid));
		}

		row.Add(FCString::Atof(*line));

		data.Add(row);
	}

	return data;
}

TArray<TArray<FVector2D>> AMapGenerator::getEdges(TArray<TArray<float>> map)
{
	TArray<TArray<FVector2D>> edges;

	FVector2D init;
	FVector2D startLine;
	FVector2D endLine;
	bool newObst = true;
	for (int32 c = 0; c < map.Num(); c++) {
		if (newObst) {
			newObst = false;
			init = FVector2D(map[c][0], map[c][1]);
			startLine = FVector2D(map[c][0], map[c][1]);
			endLine = FVector2D(map[c + 1][0], map[c + 1][1]);
		} else {
			startLine = endLine;
			if (map[c][2] == 1) {
				endLine = FVector2D(map[c + 1][0], map[c + 1][1]);
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

FVector2D AMapGenerator::gridToLocation(float X, float Y) {
	return FVector2D(Y * gridSize, X * gridSize);
}

FVector2D AMapGenerator::gridToLocation(FVector2D vector) {
	return gridToLocation(vector.X, vector.Y);
}