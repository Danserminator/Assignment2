// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "SimulatedAnnealing.h"


TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::simulatedAnnealing(TArray<AAgent *> agents, TArray<FVector2D> customers)
{
	this->agents = agents;
	this->customers = customers;

	return annealing();
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::annealing()
{
	TMap<AAgent *, TArray<FVector2D>> solution = initialConfiguration();	// Initial solution
	float alpha = 0.99;	// Temperature reduction multiplier
	float beta = 1.05;	// Iteration multiplier
	float M0 = 5;		// Time until next parameter update
	float temperature = 5000;
	TMap<AAgent *, TArray<FVector2D>> currentSolution = solution;
	float currentCost = cost(currentSolution);
	TMap<AAgent *, TArray<FVector2D>> bestSolution = solution;
	float bestCost = currentCost;
	float time = 0;

	float maxTime = -1;	// WTF is this?!

	while (time > maxTime && temperature > 0.001) {
		for (int32 c = 0; c < M0; c++) {
			TMap<AAgent *, TArray<FVector2D>> newSolution = neighbour(currentSolution);
			float newCost = cost(newSolution);

			float deltaCost = newCost - currentCost;
			if (deltaCost < 0) {
				currentSolution = newSolution;
				currentCost = newCost;

				if (newCost < bestCost) {
					bestSolution = newSolution;
					bestCost = newCost;
				}
			} else if (FMath::SRand() < FMath::Exp(-(deltaCost) / temperature)) {
				currentSolution = newSolution;
				currentCost = newCost;
			}
		}

		time = time + M0;

		temperature *= alpha;

		M0 *= beta;
	}

	return bestSolution;
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::neighbour(TMap<AAgent *, TArray<FVector2D>> routes)
{
	routes = move(routes);

	routes = replaceHighestAverage(routes);
	
	return routes;
}

/*
* TODO
*/
TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::move(TMap<AAgent *, TArray<FVector2D>> routes)
{
	TArray<TTuple<FVector2D, FVector2D>> shortestDistance;

	float highest = 0;

	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<FVector2D> route = routes.FindRef(agents[c]);
		for (int32 g = 1; g < route.Num(); g++) {
			float dist = cost(route[g - 1], route[g]);

			if (shortestDistance.Num() < 5) {
				shortestDistance.Emplace(route[g - 1], route[g]);
				
				highest = (dist > highest) ? dist : highest;
			} else {
				if (dist < highest) {
					// TODO
				}
			}
		}
	}

	return TMap<AAgent *, TArray<FVector2D>>();
}

/*
* TODO
*/
TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::replaceHighestAverage(TMap<AAgent *, TArray<FVector2D>> routes)
{
	int32 numberToChange = 5;

	TArray<TTuple<float, FVector2D, AAgent *, int32>> highestAverage;


	// Find the five vertices with highest averge distance to neighbours.
	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<FVector2D> route = routes.FindRef(agents[c]);

		route.Insert(to2D(agents[c]->GetActorLocation()), 0);

		for (int32 g = 1; g < route.Num(); g++) {
			float distance = cost(route[g - 1], route[g]);

			if (g + 1 < route.Num()) {
				distance += cost(route[g], route[g + 1]);
				distance /= 2;
			}

			if (highestAverage.Num() < numberToChange) {
				highestAverage.Emplace(distance, route[g], agents[c], g-1);
			} else {
				float lowest = std::numeric_limits<float>::infinity();
				int32 index = 0;
				for (int32 i = 0; i < highestAverage.Num(); i++) {
					if (highestAverage[i].Get<0>() < lowest) {
						lowest = highestAverage[i].Get<0>();
						index = i;
					}
				}

				if (lowest < distance) {
					highestAverage[index] = TTuple<float, FVector2D, AAgent *, int32>(distance, route[g], agents[c], g-1);
				}
			}
		}
	}

	TArray<FVector2D> removed;

	// Remove the five we found above.
	for (int32 c = 0; c < highestAverage.Num(); c++) {
		AAgent * agent = highestAverage[c].Get<2>();

		int32 index = highestAverage[c].Get<3>();

		TArray<FVector2D> route = routes.FindRef(agent);

		route.RemoveAt(index);

		FVector2D location = highestAverage[c].Get<1>();

		removed.Add(location);

		routes.Emplace(agent, route);
	}

	// Get five random agents.
	TArray<AAgent *> randomAgents;

	for (int32 c = 0; c < numberToChange; c++) {
		// Do they all need to be different?!
		int32 randNum = FMath::RandRange(0, agents.Num() - 1);
		randomAgents.Add(agents[randNum]);
	}

	// Insert the five removed costumers into the five random selected agents.
	// In a way that minimizing the cost.
	TMap<FVector2D, AAgent *> best = insertBest(removed, randomAgents);
	

	for (int32 c = 0; c < removed.Num(); c++) {
		AAgent * agent = best.FindRef(removed[c]);

		TArray<FVector2D> route = routes.FindRef(agent);

		route.Insert(to2D(agent->GetActorLocation()), 0);

		route = addToRoute(route, removed[c]);

		route.RemoveAt(0);

		routes.Emplace(agent, route);
	}

	return routes;
}

TMap<FVector2D, AAgent *> ASimulatedAnnealing::insertBest(TArray<FVector2D> locations, TArray<AAgent *> agent)
{
	float bestCost = std::numeric_limits<float>::infinity();
	bool first = true;
}

float ASimulatedAnnealing::ifAddedCost(TArray<FVector2D> route, FVector2D costumer)
{
	float lowestCost = std::numeric_limits<float>::infinity();

	for (int32 c = 1; c < route.Num(); c++) {
		route.Insert(costumer, c);

		float newCost = cost(route);

		if (newCost < lowestCost) {
			lowestCost = newCost;
		}

		route.RemoveAt(c);
	}


	return lowestCost;
}

/*
* Insert costumer into the optimal index of the route.
*/
TArray<FVector2D> ASimulatedAnnealing::addToRoute(TArray<FVector2D> route, FVector2D costumer)
{
	float lowestCost = std::numeric_limits<float>::infinity();
	int32 index = 1;

	for (int32 c = 1; c < route.Num(); c++) {
		route.Insert(costumer, c);

		float newCost = cost(route);

		if (newCost < lowestCost) {
			lowestCost = newCost;
			index = c;
		}

		route.RemoveAt(c);
	}
	
	route.Insert(costumer, index);
	
	return route;
}

/*
* TODO: Check if this works
*/
TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::initialConfiguration()
{
	TMap<AAgent *, TArray<FVector2D>> routes;

	float customerPerAgent = customers.Num() / agents.Num();

	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<FVector2D> route;
		for (int32 g = c * customerPerAgent; g < (c + 1) * customerPerAgent; g++) {
			route.Add(customers[g]);
		}

		// TODO: Make sure so that every customer is apart of a route.
		// And also so the same costumer is not part of multiple routes.

		routes.Emplace(agents[c], route);
	}

	return routes;
}

/*
* Returns the shortest distance between two points without colliding into an
* obstacle.
*/
float ASimulatedAnnealing::cost(FVector2D start, FVector2D stop) const
{
	// TODO: obstacle avoidens (visibility graph + A*).
	return FVector2D::Distance(start, stop);
}

float ASimulatedAnnealing::cost(TArray<FVector2D> route) const
{
	float totalCost = 0;
	for (int32 c = 1; c < route.Num(); c++) {
		totalCost += cost(route[c - 1], route[c]);
	}
	return totalCost;
}

/*
* Returns the total cost of all routes.
*/
float ASimulatedAnnealing::cost(TMap<AAgent *, TArray<FVector2D>> routes) const
{
	float totalCost = 0;
	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<FVector2D> route = routes.FindRef(agents[c]);

		totalCost += cost(to2D(agents[c]->GetActorLocation()), route[0]);

		totalCost += cost(route);
	}

	return totalCost;
}

FVector2D ASimulatedAnnealing::to2D(FVector vector) const
{
	return FVector2D(vector.X, vector.Y);
}