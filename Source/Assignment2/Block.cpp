// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "Block.h"

void ABlock::changeScale(float distance) {
	FVector scale = GetActorScale3D();
	SetActorScale3D(FVector(distance, scale.Y, scale.Z));
}