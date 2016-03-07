// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DubinsPath.h"

TArray<FVector2D> DubinsPath::getPath(TArray<FVector2D> waypoints, FVector2D location, float rotation, float maxAngle, float length, AVisibilityGraph * vGraph, float errorTolerance)
{
	// Remove the first one if it is in the current location
	bool removed = false;
	do {
		removed = false;
		if (waypoints.Num() > 0) {
			if (waypoints[0].Equals(location, 0.1)) {
				waypoints.RemoveAt(0);
				removed = true;
			}
		}
	} while (removed);

	if (waypoints.Num() == 0) {
		return TArray<FVector2D>();	// We are not going anywhere
	}


	TArray<FVector2D> newWaypoints;

	float turnRadius = (length / UKismetMathLibrary::Sin(UKismetMathLibrary::DegreesToRadians(maxAngle))) + (length / UKismetMathLibrary::DegTan(maxAngle)); // length / UKismetMathLibrary::Sin(UKismetMathLibrary::DegreesToRadians(maxAngle));
	turnRadius /= 2;

	FVector2D currentLocation = location;
	float currentRotation = UKismetMathLibrary::DegreesToRadians(rotation);

	for (int32 c = 1; c < waypoints.Num(); c++) {
		FVector2D currentWaypoint = waypoints[c - 1];
		FVector2D nextWaypoint = waypoints[c];			// We want to be facing this waypoint when we reach the current waypoint

		float wantedRotation = getRotation(currentWaypoint, nextWaypoint);

		TTuple<FVector2D, FVector2D> currentCircles = calculatedCircles(currentLocation, currentRotation, turnRadius);

		bool done = false;
		float targetRotation = 0;
		for (int32 r = 0; r <= 180 && !done; r++) {
			for (int32 g = -1; g <= 1 && !done; g += 2) {
				if ((r == 0 || r == 180) && g == 1) {
					continue;
				}

				targetRotation = UKismetMathLibrary::DegreesToRadians(positiveAngle(positiveAngle(wantedRotation) + (r * g)));

				TTuple<FVector2D, FVector2D> waypointCircles = calculatedCircles(currentWaypoint, targetRotation, turnRadius);
			
				TArray<TTuple<float, TArray<FVector2D>>> routes;

				// CSC
				TArray<TTuple<FVector2D, FVector2D>> RRTangents = tangentLines(currentCircles.Get<RIGHT>(), waypointCircles.Get<RIGHT>(), turnRadius);
				TArray<TTuple<FVector2D, FVector2D>> LLTangents = tangentLines(currentCircles.Get<LEFT>(), waypointCircles.Get<LEFT>(), turnRadius);
				TArray<TTuple<FVector2D, FVector2D>> RLTangents = tangentLines(currentCircles.Get<RIGHT>(), waypointCircles.Get<LEFT>(), turnRadius);
				TArray<TTuple<FVector2D, FVector2D>> LRTangents = tangentLines(currentCircles.Get<LEFT>(), waypointCircles.Get<RIGHT>(), turnRadius);
			
				if (RRTangents.Num() > 0) {
					routes.Add(RSR(RRTangents[0], currentLocation, currentWaypoint, currentCircles.Get<RIGHT>(), waypointCircles.Get<RIGHT>(), turnRadius));
				}

				if (LLTangents.Num() > 1) {
					routes.Add(LSL(LLTangents[1], currentLocation, currentWaypoint, currentCircles.Get<LEFT>(), waypointCircles.Get<LEFT>(), turnRadius));
				}

				if (RLTangents.Num() > 2) {
					routes.Add(RSL(RLTangents[2], currentLocation, currentWaypoint, currentCircles.Get<RIGHT>(), waypointCircles.Get<LEFT>(), turnRadius));
				}

				if (LRTangents.Num() > 3) {
					routes.Add(LSR(LRTangents[3], currentLocation, currentWaypoint, currentCircles.Get<LEFT>(), waypointCircles.Get<RIGHT>(), turnRadius));
				}

				// CCC
				
				float theta = 0;
				float D = normal(currentCircles.Get<RIGHT>(), waypointCircles.Get<RIGHT>());

				// RLR
				if (D < 4 * turnRadius) {
					theta = UKismetMathLibrary::Acos(D / (4 * turnRadius));

					theta += UKismetMathLibrary::Atan2(waypointCircles.Get<RIGHT>().Y - currentCircles.Get<RIGHT>().Y, waypointCircles.Get<RIGHT>().X - currentCircles.Get<RIGHT>().X);

					routes.Add(RLR(theta, currentLocation, currentWaypoint, currentCircles.Get<RIGHT>(), waypointCircles.Get<RIGHT>(), turnRadius));
				}


				// LRL
				D = normal(currentCircles.Get<LEFT>(), waypointCircles.Get<LEFT>());

				if (D < 4 * turnRadius) {
					theta = UKismetMathLibrary::Acos(D / (4 * turnRadius));

					theta += UKismetMathLibrary::Atan2(waypointCircles.Get<LEFT>().Y - currentCircles.Get<LEFT>().Y, waypointCircles.Get<LEFT>().X - currentCircles.Get<LEFT>().X) - theta;

					routes.Add(LRL(theta, currentLocation, currentWaypoint, currentCircles.Get<LEFT>(), waypointCircles.Get<LEFT>(), turnRadius));
				}


				// Sort the routes based on distance, and take the shortest route that does not collided (if any does).
				routes.Sort([](const auto& One, const auto& Two) {
					return One.Get<0>() < Two.Get<0>();
				});

				for (int32 i = 0; i < routes.Num(); i++) {
					TArray<FVector2D> route = routes[i].Get<1>();

					bool valid = validRoute(route, currentLocation, currentRotation, maxAngle, errorTolerance, length, vGraph);
					if (valid) {
						for (int32 j = 0; j < route.Num(); j++) {
							newWaypoints.Add(route[j]);
						}
						//newWaypoints.Append(route);
						done = true;	// We have found the best path here!
						break;
					}
				}
			}
		}

		if (!done) {
			return TArray<FVector2D>();
		}

		currentLocation = currentWaypoint;
		currentRotation = targetRotation;
	}

	// Only one waypoint remaining, we do not care about the
	// direction we are facing when reaching it.
	{
		FVector2D currentWaypoint = waypoints[waypoints.Num() - 1];

		newWaypoints.Add(currentWaypoint);	// Can we just add it?!
	}

	return newWaypoints;
}

bool DubinsPath::validRoute(TArray<FVector2D> route, FVector2D currentLocation, float currentRotation, float maxAngle, float errorTolerance, float length, AVisibilityGraph * vGraph)
{
	if (route.Num() == 0) {
		return false;
	}

	float stepSize = 0.02;

	float v = 10;

	TArray<TArray<FVector2D>> edges = vGraph->getEdges();

	for (int32 c = 0; c < route.Num(); c++) {
		FVector2D target = route[c];

		while (!currentLocation.Equals(target, errorTolerance)) {
			float rotation = positiveAngle(UKismetMathLibrary::FindLookAtRotation(to3D(currentLocation), to3D(target)).Yaw);

			rotation -= currentRotation;

			float curMaxAngle = maxAngle * stepSize;

			rotation = UKismetMathLibrary::ClampAngle(positiveAngle(rotation), -curMaxAngle, curMaxAngle);

			rotation = UKismetMathLibrary::DegTan(positiveAngle(rotation));

			rotation *= v / length;

			rotation += UKismetMathLibrary::DegreesToRadians(currentRotation);

			currentRotation = UKismetMathLibrary::RadiansToDegrees(rotation);

			FVector2D newLocation(v * UKismetMathLibrary::DegCos(currentRotation) * stepSize, v * UKismetMathLibrary::DegSin(currentRotation) * stepSize);

			newLocation += currentLocation;

			//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Magenta, FString::Printf(TEXT("%s -> %s"), *currentLocation.ToString(), *target.ToString()));

			if (!vGraph->canSee(currentLocation, newLocation, edges)) {
				return false;
			} else {
				currentLocation = newLocation;
			}
		}
	}

	return true;
}

float DubinsPath::normal(FVector2D start, FVector2D end)
{
	return UKismetMathLibrary::Sqrt((end.X - start.X) * (end.X - start.X) + (end.Y - start.Y) * (end.Y - start.Y));
}

float DubinsPath::arcLength(FVector2D center, FVector2D start, FVector2D end, float radius, bool left)
{
	FVector2D vec1, vec2;

	vec1.X = start.X - center.X;
	vec1.Y = start.Y - center.Y;

	vec2.X = end.X - center.X;
	vec2.Y = end.Y - center.Y;

	float theta = UKismetMathLibrary::Atan2(vec2.Y, vec2.X) - UKismetMathLibrary::Atan2(vec1.Y, vec1.X);

	if (theta < -1e-6 && left) {
		theta += 2 * UKismetMathLibrary::GetPI();;
	} else if (theta > 1e-6 && !left) {
		theta -= 2 * UKismetMathLibrary::GetPI();
	}

	return UKismetMathLibrary::Abs(theta * radius);
}

TTuple<float, TArray<FVector2D>> DubinsPath::RSR(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius)
{
	float length = arcLength(circleStart, start, tangent.Get<0>(), radius, false);

	length += normal(tangent.Get<0>(), tangent.Get<1>());

	length += arcLength(circleEnd, tangent.Get<1>(), end, radius, false);

	TArray<FVector2D> path;

	path.Add(tangent.Get<0>());
	path.Add(tangent.Get<1>());
	path.Add(end);

	return TTuple<float, TArray<FVector2D>>(length, path);
}

TTuple<float, TArray<FVector2D>> DubinsPath::LSL(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius)
{
	float length = arcLength(circleStart, start, tangent.Get<0>(), radius, true);

	length += normal(tangent.Get<0>(), tangent.Get<1>());

	length += arcLength(circleEnd, tangent.Get<1>(), end, radius, true);

	TArray<FVector2D> path;

	path.Add(tangent.Get<0>());
	path.Add(tangent.Get<1>());
	path.Add(end);

	return TTuple<float, TArray<FVector2D>>(length, path);
}

TTuple<float, TArray<FVector2D>> DubinsPath::RSL(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius)
{
	float length = arcLength(circleStart, start, tangent.Get<0>(), radius, false);

	length += normal(tangent.Get<0>(), tangent.Get<1>());

	length += arcLength(circleEnd, tangent.Get<1>(), end, radius, true);

	TArray<FVector2D> path;

	path.Add(tangent.Get<0>());
	path.Add(tangent.Get<1>());
	path.Add(end);

	return TTuple<float, TArray<FVector2D>>(length, path);
}

TTuple<float, TArray<FVector2D>> DubinsPath::LSR(TTuple<FVector2D, FVector2D> tangent, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius)
{
	float length = arcLength(circleStart, start, tangent.Get<0>(), radius, true);

	length += normal(tangent.Get<0>(), tangent.Get<1>());

	length += arcLength(circleEnd, tangent.Get<1>(), end, radius, false);

	TArray<FVector2D> path;

	path.Add(tangent.Get<0>());
	path.Add(tangent.Get<1>());
	path.Add(end);

	return TTuple<float, TArray<FVector2D>>(length, path);
}

TTuple<float, TArray<FVector2D>> DubinsPath::RLR(float theta, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius)
{
	FVector2D lCircle(circleStart.X + (2 * radius * UKismetMathLibrary::Cos(theta)), circleStart.Y + (2 * radius * UKismetMathLibrary::Sin(theta)));

	FVector2D startTangent((lCircle.X + circleStart.X) / 2, (lCircle.Y + circleStart.Y) / 2);

	FVector2D endTangent((lCircle.X + circleEnd.X) / 2, (lCircle.Y + circleEnd.Y) / 2);

	float length = arcLength(circleStart, start, startTangent, radius, false);

	length += arcLength(lCircle, startTangent, endTangent, radius, true);

	length += arcLength(circleEnd, endTangent, end, radius, false);


	TArray<FVector2D> path;

	path.Add(startTangent);
	path.Add(endTangent);
	path.Add(end);

	return TTuple<float, TArray<FVector2D>>(length, path);
}

TTuple<float, TArray<FVector2D>> DubinsPath::LRL(float theta, FVector2D start, FVector2D end, FVector2D circleStart, FVector2D circleEnd, float radius)
{
	FVector2D rCircle(circleStart.X + (2 * radius * UKismetMathLibrary::Cos(theta)), circleStart.Y + (2 * radius * UKismetMathLibrary::Sin(theta)));

	FVector2D startTangent((rCircle.X + circleStart.X) / 2, (rCircle.Y + circleStart.Y) / 2);

	FVector2D endTangent((rCircle.X + circleEnd.X) / 2, (rCircle.Y + circleEnd.Y) / 2);

	float length = arcLength(circleStart, start, startTangent, radius, true);

	length += arcLength(rCircle, startTangent, endTangent, radius, false);

	length += arcLength(circleEnd, endTangent, end, radius, true);


	TArray<FVector2D> path;

	path.Add(startTangent);
	path.Add(endTangent);
	path.Add(end);

	return TTuple<float, TArray<FVector2D>>(length, path);
}

TArray<TTuple<FVector2D, FVector2D>> DubinsPath::tangentLines(FVector2D circle1, FVector2D circle2, float radius)
{
	TArray<TTuple<FVector2D, FVector2D>> tangentLines;

	float x1 = circle1.X;
	float y1 = circle1.Y;
	float x2 = circle2.X;
	float y2 = circle2.Y;

	float distanceSquared = ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1));

	if (distanceSquared < 0) {	// 0 because they will always have the same radius
		// ???

		if (distanceSquared != radius && distanceSquared < radius) {
			return tangentLines;	// ERROR!!! kind of...
		}
	}

	float d = UKismetMathLibrary::Sqrt(distanceSquared);

	float vx = (x2 - x1) / d;
	float vy = (y2 - y1) / d;

	for (int32 sign1 = +1; sign1 >= -1; sign1 -= 2) {
		float c = (radius - sign1 * radius) / d;
		
		if (c * c > 1) {
			continue;
		}

		float h = UKismetMathLibrary::Sqrt(UKismetMathLibrary::FMax(0, 1 - c * c));

		for (int32 sign2 = +1; sign2 >= -1; sign2 -= 2) {
			float nx = vx * c - sign2 * h * vy;
			float ny = vy * c + sign2 * h * vx;

			tangentLines.Emplace(FVector2D(x1 + radius * nx, y1 + radius * ny), FVector2D(x2 + sign1 * radius * nx, y2 + sign1 * radius * ny));
		}

	}

	return tangentLines;
}

TTuple<FVector2D, FVector2D> DubinsPath::calculatedCircles(FVector2D location, float rotation, float turnRadius)
{
	// Right circle
	FVector2D rightCircle(location.X + turnRadius * UKismetMathLibrary::Cos(rotation - (UKismetMathLibrary::GetPI() / 2)), location.Y + turnRadius * UKismetMathLibrary::Sin(rotation - (UKismetMathLibrary::GetPI() / 2)));

	// Left circle
	FVector2D leftCircle(location.X + turnRadius * UKismetMathLibrary::Cos(rotation + (UKismetMathLibrary::GetPI() / 2)), location.Y + turnRadius * UKismetMathLibrary::Sin(rotation + (UKismetMathLibrary::GetPI() / 2)));

	return TTuple<FVector2D, FVector2D>(rightCircle, leftCircle);
}


float DubinsPath::getRotation(FVector2D start, FVector2D target)
{
	return UKismetMathLibrary::FindLookAtRotation(to3D(start), to3D(target)).Yaw;
}