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

// Called every frame
void AAgent::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AAgent::setTarget(FVector2D t) {
	target = t;

	FVector loc = GetActorLocation();
	FVector direction = FVector(target, 0) - FVector(loc.X, loc.Y, 0);
	FRotator rot = FRotationMatrix::MakeFromX(direction).Rotator();
	SetActorRotation(rot);

	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("{%f, %f, %f}\r\n"), rot.Roll, rot.Pitch, rot.Yaw));

	SpawnDefaultController();
}