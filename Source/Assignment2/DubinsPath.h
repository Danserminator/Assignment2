// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "General.h"
#include "VisibilityGraph.h"
#include "Kismet/KismetMathLibrary.h"

#define RIGHT 0
#define LEFT 1

/**
 * 
 */
class ASSIGNMENT2_API DubinsPath
{
public:
	static TArray<FVector2D> getPath(TArray<FVector2D> waypoints, FVector2D location, float rotation, float maxAngle, float length, AVisibilityGraph * vGraph, float errorTolerance);

	static bool validRoute(TArray<FVector2D> route, FVector2D currentLocation, float currentRotation, float maxAngle, float errorTolerance, float length, AVisibilityGraph * vGraph);

	static TTuple<FVector2D, FVector2D> calculatedCircles(FVector2D location, float rotation, float turnRadius);

	static TArray<TTuple<FVector2D, FVector2D>> tangentLines(FVector2D circle1, FVector2D circle2, float radius);

	static float normal(FVector2D start, FVector2D end);

	static float arcLength(FVector2D center, FVector2D start, FVector2D end, float radius, bool left);

	static TTuple<float, TArray<FVector2D>> RSR(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius);

	static TTuple<float, TArray<FVector2D>> LSL(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius);

	static TTuple<float, TArray<FVector2D>> RSL(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius);

	static TTuple<float, TArray<FVector2D>> LSR(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius);

	static TTuple<float, TArray<FVector2D>> RLR(float theta, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius);

	static TTuple<float, TArray<FVector2D>> LRL(float theta, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius);

	static float getRotation(FVector2D start, FVector2D target);
};
