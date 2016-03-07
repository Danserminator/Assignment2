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
	if (angle >= 0)	return angle;

	return FMath::Fmod((angle + 360), 360);
}

float angleDiff(float a1, float a2)
{
	float delta = FMath::Max(a1, a2) - FMath::Min(a1, a2);
	if (180 < delta) {
		delta = 360 - delta;
	}

	return delta;
}