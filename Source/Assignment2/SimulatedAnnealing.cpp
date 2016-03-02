// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "SimulatedAnnealing.h"

//#define OUTPUT

void ASimulatedAnnealing::simulatedAnnealing(AVisibilityGraph * graph, TArray<AAgent *> agents, TArray<FVector2D> inputCustomers,
	float temperature, float alpha, float beta, float M0, float maxTime)
{
	this->graph = graph;
	this->temperature = temperature;
	this->alpha = alpha;	// Temperature reduction multiplier
	this->beta = beta;	// Iteration multiplier
	this->M0 = M0;		// Time until next parameter update
	this->maxTime = maxTime;

#ifdef OUTPUT1
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Hej")));
#endif

	this->agents = agents;
	this->customers = inputCustomers;

#ifdef OUTPUT1
	FString str;
	str.Append("Customers: ");
	for (int32 c = 0; c < inputCustomers.Num(); c++) {
		str.Append("(");
		str.Append(FString::SanitizeFloat(inputCustomers[c].Y));
		str.Append(", ");
		str.Append(FString::SanitizeFloat(inputCustomers[c].X));
		str.Append(") ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
#endif

	routes = annealing();

	done = true;

#ifdef OUTPUT1
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Done")));
#endif


#ifdef OUTPUT
	writeRoutesToFile("Routes.txt");
	UE_LOG(LogTemp, Warning, TEXT("Final routes - Total cost: %f"), cost(routes));
	for (int32 c = 0; c < agents.Num(); c++) {
		AAgent * agent = agents[c];

		TArray<FVector2D> route = routes.FindRef(agents[c]);

		FString str;

		str.Append("Agent (");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().Y));
		str.Append(", ");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().X));
		str.Append("): ");

		for (int32 g = 0; g < route.Num()-1; g++) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[g].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[g].X));
			str.Append(") -> ");
		}

		if (route.Num() != 0) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].X));
			str.Append(")");
		}

		str.Append(". Cost: ");
		str.Append(FString::SanitizeFloat(cost(agent, route)));

		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	}
#endif
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::getRoutes()
{
	if (!done) {
		//simulatedAnnealing();
	}

	return routes;
}

TArray<FVector2D> ASimulatedAnnealing::getRoute(AAgent * agent)
{
	if (!done) {
		//simulatedAnnealing();
	}

	return routes.FindRef(agent);
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::annealing()
{
	TMap<AAgent *, TArray<FVector2D>> solution = initialConfiguration();	// Initial solution
	TMap<AAgent *, TArray<FVector2D>> currentSolution = solution;
	float currentCost = cost(currentSolution);
	TMap<AAgent *, TArray<FVector2D>> bestSolution = solution;
	float bestCost = currentCost;
	
	time_t begin;

	time(&begin);

	time_t current;

	time(&current);

	while (maxTime > difftime(current, begin) && temperature > 0.001) {
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
			}
			else if (FMath::SRand() < FMath::Exp(-(deltaCost) / temperature)) {
				currentSolution = newSolution;
				currentCost = newCost;
			}
		}

		//time = time + M0;

		temperature *= alpha;

		M0 *= beta;

		time(&current);
	}

	return bestSolution;
}

/*
* TODO: Check if this works + make it into the greedy algorithm
*/
TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::initialConfiguration()
{
	TMap<AAgent *, TArray<FVector2D>> routes;

	for (int32 c = 0; c < customers.Num(); c++) {
		float closestDist = std::numeric_limits<float>::infinity();
		AAgent * closestAgent = agents[0];

		for (int32 g = 0; g < agents.Num(); g++) {
			AAgent * agent = agents[g];

			float dist = cost(to2D(agent->GetActorLocation()), customers[c]);

			if (dist < closestDist) {
				closestDist = dist;
				closestAgent = agent;
			}
		}

		TArray<FVector2D> route;
		if (routes.Contains(closestAgent)) {
			route = routes.FindRef(closestAgent);
		}

		route.Add(customers[c]);
		routes.Emplace(closestAgent, route);
	}

	/*
	float customerPerAgent = customers.Num() / agents.Num();

	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<FVector2D> route;
		for (int32 g = c * customerPerAgent; g < (c + 1) * customerPerAgent; g++) {
			route.Add(customers[g]);
		}

		// TODO: Make sure so that every customer is apart of a route.
		// And also so the same customer is not part of multiple routes.

		routes.Emplace(agents[c], route);
	}
	*/
	return routes;
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::neighbour(TMap<AAgent *, TArray<FVector2D>> routes)
{
#ifdef OUTPUT1
	UE_LOG(LogTemp, Warning, TEXT("Neighbour 1"));
	for (int32 c = 0; c < agents.Num(); c++) {
		AAgent * agent = agents[c];

		TArray<FVector2D> route = routes.FindRef(agents[c]);

		FString str;

		str.Append("Agent (");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().Y));
		str.Append(", ");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().X));
		str.Append("): ");

		for (int32 g = 0; g < route.Num() - 1; g++) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[g].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[g].X));
			str.Append(") -> ");
		}

		if (route.Num() != 0) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].X));
			str.Append(")");
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	}
#endif
	routes = move(routes);
#ifdef OUTPUT1
	UE_LOG(LogTemp, Warning, TEXT("Neighbour 2"));
	for (int32 c = 0; c < agents.Num(); c++) {
		AAgent * agent = agents[c];

		TArray<FVector2D> route = routes.FindRef(agents[c]);

		FString str;

		str.Append("Agent (");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().Y));
		str.Append(", ");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().X));
		str.Append("): ");

		for (int32 g = 0; g < route.Num() - 1; g++) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[g].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[g].X));
			str.Append(") -> ");
		}

		if (route.Num() != 0) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].X));
			str.Append(")");
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	}
#endif
	routes = replaceHighestAverage(routes);
#ifdef OUTPUT1
	UE_LOG(LogTemp, Warning, TEXT("Neighbour 3"));
	for (int32 c = 0; c < agents.Num(); c++) {
		AAgent * agent = agents[c];

		TArray<FVector2D> route = routes.FindRef(agents[c]);

		FString str;

		str.Append("Agent (");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().Y));
		str.Append(", ");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().X));
		str.Append("): ");

		for (int32 g = 0; g < route.Num() - 1; g++) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[g].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[g].X));
			str.Append(") -> ");
		}

		if (route.Num() != 0) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].Y));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].X));
			str.Append(")");
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	}
#endif
	return routes;
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::move(TMap<AAgent *, TArray<FVector2D>> routes)
{
	if (numberToChange * 2 >= customers.Num()) {
		return routes;
	}

	// Find the five pair of customers with shortest distance between them. 
	TArray<TTuple<float, AAgent *, FVector2D, FVector2D, bool>> shortestDistance;

	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<FVector2D> route = routes.FindRef(agents[c]);

		if (route.Num() == 0) {
			continue;
		}

		AAgent * agent = agents[c];

		float dist = cost(to2D(agent->GetActorLocation()), route[0]);
		
		FVector2D location1 = to2D(agent->GetActorLocation());
		FVector2D location2 = route[0];

		
		bool haveAgent = true;
		if (shortestDistance.Num() < numberToChange) {
			shortestDistance.Emplace(dist, agent, location1, location2, haveAgent);
		} else {
			for (int32 i = 0; i < numberToChange; i++) {
				if (dist < shortestDistance[i].Get<0>()) {
					auto temp = shortestDistance[i];
					shortestDistance[i] = TTuple<float, AAgent *, FVector2D, FVector2D, bool>(dist, agent, location1, location2, haveAgent);

					dist = temp.Get<0>();
					agent = temp.Get<1>();
					location1 = temp.Get<2>();
					location2 = temp.Get<3>();
					haveAgent = temp.Get<4>();
				}
			}
		}

		for (int32 g = 1; g < route.Num(); g++) {
			dist = cost(route[g - 1], route[g]);

			agent = agents[c];
			location1 = route[g - 1];
			location2 = route[g];

			haveAgent = false;

			if (shortestDistance.Num() < numberToChange) {
				shortestDistance.Emplace(dist, agent, location1, location2, haveAgent);
			}
			else {
				for (int32 i = 0; i < numberToChange; i++) {
					if (dist < shortestDistance[i].Get<0>()) {
						auto temp = shortestDistance[i];
						shortestDistance[i] = TTuple<float, AAgent *, FVector2D, FVector2D, bool>(dist, agent, location1, location2, haveAgent);

						dist = temp.Get<0>();
						agent = temp.Get<1>();
						location1 = temp.Get<2>();
						location2 = temp.Get<3>();
						haveAgent = temp.Get<4>();
					}
				}
			}
		}
	}

	// Select five random customers that exclude the agent and the customer at "index2".
	TArray<FVector2D> randomcustomers;

	TArray<FVector2D> pickFrom = customers;

	while (randomcustomers.Num() < numberToChange) {
		int32 customerIndex = FMath::RandRange(0, pickFrom.Num() - 1);		// TODO: make better, fix a list of only the available.

		FVector2D customer = pickFrom[customerIndex];

		pickFrom.RemoveAt(customerIndex);

		// Check if this is a valid choice.
		bool valid = true;
		for (int32 i = 0; i < shortestDistance.Num(); i++) {
			auto pair = shortestDistance[i];

			if (!pair.Get<4>()) {	// Can not move an agent.
				if (pair.Get<3>().Equals(customer, 0)) {
					valid = false;
					break;
				}
			}
		}

		if (valid) {
			randomcustomers.Add(customer);
		
			// Remove from route.
			for (int32 c = 0; c < agents.Num(); c++) {
				TArray<FVector2D> route = routes.FindRef(agents[c]);

				if (route.Contains(customer)) {
					route.Remove(customer);
					routes.Emplace(agents[c], route);
					break;
				}
			}
		}
	}

	// Insert the random customers to random routes.
	for (int32 c = 0; c < randomcustomers.Num(); c++) {
		int32 agentIndex = FMath::RandRange(0, agents.Num() - 1);

		AAgent * agent = agents[agentIndex];

		TArray<FVector2D> route = routes.FindRef(agent);

		FVector2D customer = randomcustomers[c];

		route = addToRoute(agent, route, customer);

		routes.Emplace(agent, route);
	}


	return routes;
}

TArray<FVector2D> ASimulatedAnnealing::addToRoute(AAgent * agent, TArray<FVector2D> route, FVector2D customer)
{
	if (route.Num() == 0) {
		route.Add(to2D(agent->GetActorLocation()));
	} else {
		route.Insert(to2D(agent->GetActorLocation()), 0);
	}

	route = addToRoute(route, customer);

	route.RemoveAt(0);

	return route;
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::replaceHighestAverage(TMap<AAgent *, TArray<FVector2D>> routes)
{
	if (numberToChange * 2 >= customers.Num()) {
		return routes;
	}

	TArray<TTuple<float, FVector2D, AAgent *>> highestAverage;


	// Find the five vertices with highest average distance to neighbours.
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
				highestAverage.Emplace(distance, route[g], agents[c]);
			}
			else {
				float lowest = std::numeric_limits<float>::infinity();
				int32 index = 0;
				for (int32 i = 0; i < highestAverage.Num(); i++) {
					if (highestAverage[i].Get<0>() < lowest) {
						lowest = highestAverage[i].Get<0>();
						index = i;
					}
				}

				if (lowest < distance) {
					highestAverage[index] = TTuple<float, FVector2D, AAgent *>(distance, route[g], agents[c]);
				}
			}
		}

		route.RemoveAt(0);
	}

#ifdef OUTPUT1
	UE_LOG(LogTemp, Warning, TEXT("Found closest"));
#endif

	TArray<FVector2D> removed;

	// Remove the five we found above.
	for (int32 c = 0; c < highestAverage.Num(); c++) {
		AAgent * agent = highestAverage[c].Get<2>();

		TArray<FVector2D> route = routes.FindRef(agent);

		FVector2D location = highestAverage[c].Get<1>();

		route.Remove(location);

		removed.Add(location);

		routes.Emplace(agent, route);
	}

#ifdef OUTPUT1
	UE_LOG(LogTemp, Warning, TEXT("Removed closest"));
#endif

	// Get five random agents.
	TArray<AAgent *> randomAgents;

	for (int32 c = 0; c < numberToChange; c++) {
		// Do they all need to be different?!
		int32 randNum = FMath::RandRange(0, agents.Num() - 1);
		randomAgents.Add(agents[randNum]);
	}

#ifdef OUTPUT1
	UE_LOG(LogTemp, Warning, TEXT("Got random agents"));
#endif

	// Insert the five removed customers into the five random selected agents.
	// In a way that minimizing the cost.
	return insertBest(routes, removed, randomAgents);

	/*
	for (int32 c = 0; c < removed.Num(); c++) {
	AAgent * agent = best.FindRef(removed[c]);

	TArray<FVector2D> route = routes.FindRef(agent);

	route.Insert(to2D(agent->GetActorLocation()), 0);

	route = addToRoute(route, removed[c]);

	route.RemoveAt(0);

	routes.Emplace(agent, route);
	}

	return routes;
	*/
}

TMap<AAgent *, TArray<FVector2D>> ASimulatedAnnealing::insertBest(TMap<AAgent *, TArray<FVector2D>> routes, TArray<FVector2D> customers, TArray<AAgent *> agents)
{
	if (customers.Num() == 0) {
		return routes;
	}

	TMap<AAgent *, TArray<FVector2D>> bestRoutes;
	float bestCost = std::numeric_limits<float>::infinity();

	for (int32 c = 0; c < customers.Num(); c++) {
		FVector2D customer = customers[c];

		customers.RemoveAt(c);

		for (int32 g = 0; g < agents.Num(); g++) {
			AAgent * agent = agents[g];

			TArray<FVector2D> route = routes.FindRef(agent);

			TArray<FVector2D> newRoute = addToRoute(agent, route, customer);

			agents.RemoveAt(g);

			routes.Emplace(agent, newRoute);

			TMap<AAgent *, TArray<FVector2D>> newRoutes = insertBest(routes, customers, agents);

			float newCost = cost(newRoutes);

			if (newCost < bestCost) {
				bestCost = newCost;
				bestRoutes = newRoutes;
			}

			routes.Emplace(agent, route);

			agents.Insert(agent, g);
		}

		customers.Insert(customer, c);
	}

	return bestRoutes;
}


float ASimulatedAnnealing::ifAddedCost(TArray<FVector2D> route, FVector2D customer)
{
	float lowestCost = std::numeric_limits<float>::infinity();

	for (int32 c = 1; c < route.Num(); c++) {
		route.Insert(customer, c);

		float newCost = cost(route);

		if (newCost < lowestCost) {
			lowestCost = newCost;
		}

		route.RemoveAt(c);
	}


	return lowestCost;
}

/*
* Insert customer into the optimal index of the route.
*/
TArray<FVector2D> ASimulatedAnnealing::addToRoute(TArray<FVector2D> route, FVector2D customer)
{
	float lowestCost = std::numeric_limits<float>::infinity();
	int32 index = 1;

	for (int32 c = 1; c < route.Num(); c++) {
		route.Insert(customer, c);

		float newCost = cost(route);

		if (newCost < lowestCost) {
			lowestCost = newCost;
			index = c;
		}

		route.RemoveAt(c);
	}

	route.Add(customer);
	float newCost = cost(route);

	if (newCost < lowestCost) {
		return route;
	} else {
		route.Remove(customer);
	}

	route.Insert(customer, index);

	return route;
}

/*
* Returns the shortest distance between two points without colliding into an
* obstacle.
*/
float ASimulatedAnnealing::cost(FVector2D start, FVector2D stop) const
{
	TArray<FVector2D> path = AStar::getPath(graph->getGraph(), graph->getVertices(), start, stop);

	float dist = 0;
	for (int32 c = 1; c < path.Num(); c++) {
		dist += FVector2D::Distance(path[c - 1], path[c]);
	}

	return dist;
}

float ASimulatedAnnealing::cost(TArray<FVector2D> route) const
{
	float totalCost = 0;
	for (int32 c = 1; c < route.Num(); c++) {
		totalCost += cost(route[c - 1], route[c]);
	}
	return totalCost;
}

float ASimulatedAnnealing::cost(AAgent * agent, TArray<FVector2D> route) const
{
	float totalCost = 0;

	if (route.Num() > 0) {
		totalCost += cost(to2D(agent->GetActorLocation()), route[0]);
	}

	totalCost += cost(route);

	return totalCost;
}

/*
* Returns the total cost of all routes.
*/
float ASimulatedAnnealing::cost(TMap<AAgent *, TArray<FVector2D>> routes) const
{
	float longestRoute = 0;
	float totalCost = 0;
	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<FVector2D> route = routes.FindRef(agents[c]);

		if (route.Num() == 0) {
			continue;
		}

		//float routeCost = cost(to2D(agents[c]->GetActorLocation()), route[0]);

		float routeCost = cost(agents[c], route);

		totalCost += routeCost;

		if (routeCost > longestRoute) {
			longestRoute = routeCost;
		}
	}

	return longestRoute;
}

void ASimulatedAnnealing::writeRoutesToFile(const FString fileName)
{
	FString str = FString("-1\t");
	str.Append(FString::SanitizeFloat(cost(routes)));
	str.Append(FString("\r\n"));

	for (int32 c = 0; c < agents.Num(); c++) {
		AAgent * agent = agents[c];

		TArray<FVector2D> route = routes.FindRef(agents[c]);

		str.Append(FString("-1\t"));	// Means new path for Matlab
		str.Append(FString::SanitizeFloat(cost(agent, route)));	// Cost for this route
		str.Append(FString("\r\n"));

		str.Append(FString::SanitizeFloat(agent->GetActorLocation().Y));
		str.Append("\t");
		str.Append(FString::SanitizeFloat(agent->GetActorLocation().X));
		str.Append("\r\n");

		for (int32 g = 0; g < route.Num() - 1; g++) {
			str.Append(FString::SanitizeFloat(route[g].Y));
			str.Append("\t");
			str.Append(FString::SanitizeFloat(route[g].X));
			str.Append("\r\n");
		}

		if (route.Num() != 0) {
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].Y));
			str.Append("\t");
			str.Append(FString::SanitizeFloat(route[route.Num() - 1].X));
			str.Append("\r\n");
		}
	}

	FString projectDir = FPaths::GameDir();
	projectDir += "Output Data/" + fileName;
	FFileHelper::SaveStringToFile(str, *projectDir);
}










/*
TMap<AAgent *, FVector2D> ASimulatedAnnealing::insertBest_help(TMap<AAgent *, TArray<FVector2D>> routes, TArray<FVector2D> customers, TArray<AAgent *> agents, TMap<AAgent *, FVector2D> & assignment)
{
if (customers.Num() == 0) {
return assignment;
}

float bestCost = std::numeric_limits<float>::infinity();
TMap<AAgent *, FVector2D> bestAssignment;

for (int32 c = 0; c < customers.Num(); c++) {
FVector2D customer = customers[c];

TMap<AAgent *, FVector2D> copyAssignment = assignment;

customers.RemoveAt(c);
for (int32 g = 0; g < agents.Num(); g++) {
AAgent * agent = agents[g];

agents.RemoveAt(g);

copyAssignment.Emplace(agent, customer);

TMap<AAgent *, FVector2D> currentAssignment = insertBest_help(customers, agents, copyAssignment);

float cost = cost(currentAssignment);

if (cost < bestCost) {
bestCost = cost;
bestAssignment = currentAssignment;
}

copyAssignment.Remove(agent);

agents.Insert(agent, g);
}

customers.Insert(customer, c);
}
}
*/