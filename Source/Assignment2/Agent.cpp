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

void AAgent::init(float r, AFormation * f, TArray<AAgent *> agents)
{
	R = r;
	formation = f;
	unseenAgents = agents;

	SpawnDefaultController();	
}