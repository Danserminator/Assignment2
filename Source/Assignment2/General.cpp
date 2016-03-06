// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "General.h"

FVector2D to2D(FVector vector)
{
	return FVector2D(vector.X, vector.Y);
}

FVector to3D(FVector2D vector)
{
	return FVector(vector, 0);
}

float positiveAngle(float angle)
{
	return FMath::Fmod((angle + 180), 360) - 180;
}