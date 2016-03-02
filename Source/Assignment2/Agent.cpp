// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "Agent.h"
#include "Engine.h"

// Sets default values
AAgent::AAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAgent::init(float r, TArray<AAgent *> agents)
{
	seeRadius = r;

	agents.Remove(this);	// Remove myself so agents only contains all other agents.

	if (seeRadius <= 0) {
		seenAgents = agents;
	} else {
		unseenAgents = agents;
	}

	SpawnDefaultController();	
}

void AAgent::findAgents()
{
	// No more agents to be found.
	if (foundAllAgents()) {
		return;
	}


	FVector2D currentLocation = to2D(GetActorLocation());

	float distance;
	for (int32 c = 0; c < unseenAgents.Num(); c++) {
		FVector2D otherLocation = to2D(unseenAgents[c]->GetActorLocation());
		distance = FVector2D::Distance(currentLocation, otherLocation);

		if (distance <= seeRadius) {
			// We can see this agent, so add it.
			seenAgents.Add(unseenAgents[c]);

			unseenAgents.RemoveAt(c);
			c--;
		}
	}
}

bool AAgent::foundAllAgents()
{
	return unseenAgents.Num() == 0;
}

int32 AAgent::numberUnseenAgents()
{
	return unseenAgents.Num();
}

TArray<AAgent *> AAgent::getSeenAgents()
{
	return seenAgents;
}

float AAgent::getSeeRadius()
{
	return seeRadius;
}