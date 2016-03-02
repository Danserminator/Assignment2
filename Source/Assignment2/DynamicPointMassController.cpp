// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicPointMassController.h"

//#define OUTPUT

<<<<<<< HEAD
ADynamicPointMassController::ADynamicPointMassController()
{
	errorTolerance = 0.01;	// Because this is the bomb!
}


// Called when the game starts or when spawned
void ADynamicPointMassController::BeginPlay() 
{
	agent = static_cast<AAgent *>(GetPawn());	// Check if can be set in constructor.
}

=======
>>>>>>> origin/master
// Called every frame
void ADynamicPointMassController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (play) {
		if (!movingFormation) {
			updateTarget();

			if (waypointReached()) {
				// TODO
			}
			else {
				acceleration = getAcceleration();

				drawLine(2 * acceleration, accelerationColor);

				float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

				acceleration *= deltaSec;

				velocity += acceleration;

				if (!everybodyKnows) {
					velocity = velocity.GetClampedToSize(-everybodyKnowsSpeed, everybodyKnowsSpeed);
				}
				else {
					velocity = velocity.GetClampedToSize(-vMax, vMax);
				}



				FVector currentLocation = agent->GetActorLocation();

				FVector newLocation = currentLocation + (velocity * deltaSec);

				setRotation();

				agent->SetActorLocation(newLocation);
				//agent->SetActorLocationAndRotation(newLocation, rotation);
			}
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("%s -> %s"), *to2D(agent->GetActorLocation()).ToString(), *target.ToString()));
			if (moveTarget || agent->numberUnseenAgents() > 0) {
				updateTarget();

				acceleration = getAcceleration();

				drawLine(2 * acceleration, accelerationColor);

				float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

				acceleration *= deltaSec;

				velocity += acceleration;
				
				velocity = velocity.GetClampedToSize(-vMax, vMax);

<<<<<<< HEAD
				FVector currentLocation = agent->GetActorLocation();

				FVector newLocation = currentLocation + (velocity * deltaSec);

				setRotation();

				agent->SetActorLocation(newLocation);
			} else {
				if (firstTry) {
					firstTry = false;
					updateTarget();
					return;
				} else if (secondTry) {
					secondTry = false;
					updateTarget();
				}

				if (waypointReached()) {
					moveTarget = true;
				}
				else {
					acceleration = getAcceleration();

					drawLine(2 * acceleration, accelerationColor);
=======
			FVector currentLocation = agent->GetActorLocation();
>>>>>>> origin/master

					float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

					acceleration *= deltaSec;

					velocity += acceleration;

					velocity = velocity.GetClampedToSize(-vMax, vMax);

					FVector currentLocation = agent->GetActorLocation();

					FVector newLocation = currentLocation + (velocity * deltaSec);

					setRotation();

					agent->SetActorLocation(newLocation);
				}
			}
		}
	}
}

FVector2D ADynamicPointMassController::getBrakeTarget()
{
	FVector2D normVelocity = to2D(velocity);
	normVelocity.Normalize();

	if (to2D(velocity) < normVelocity) {
		normVelocity = to2D(velocity);
	}

	return (to2D(agent->GetActorLocation()) + normVelocity + (errorTolerance * 0.9));
}

bool ADynamicPointMassController::waypointReached()
{
	bool stoppedPre = stopped;
	if (AModelController::waypointReached()) {
		stopped = stoppedPre;
		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		FVector2D frameAcceleration = to2D(getAcceleration()) * deltaSec;

		FVector2D frameVelocity = to2D(velocity) * deltaSec;

		if (FMath::Abs(frameVelocity.X) > FMath::Abs(frameAcceleration.X) || FMath::Abs(frameVelocity.Y) > FMath::Abs(frameAcceleration.Y)) {
			// Too high velocity for us to stop in this time frame.
			return false;
		} else {
			// Can stop in this time frame.
			velocity = FVector(0, 0, 0);
			stopped = true;
			return true;
		}
	}
	
	return false;
}

FVector ADynamicPointMassController::getAcceleration() const
{
	FVector newAcceleration;

	float rotation = getRotation(agent->GetActorLocation(), target).Yaw;

	newAcceleration = FVector(UKismetMathLibrary::DegCos(rotation), UKismetMathLibrary::DegSin(rotation), 0);

	newAcceleration = aMax * newAcceleration / mass;

	float velocityLength = UKismetMathLibrary::VSize(velocity);

	velocityLength = velocityLength * velocityLength / (aMax * 2);

	float distLeftLength = (target - to2D(agent->GetActorLocation())).Size() - safetyBuffer;
	//float distLeftLength = UKismetMathLibrary::VSize(to3D(target) - to3D(to2D(agent->GetActorLocation()))) - safetyBuffer;

	if (velocityLength >= distLeftLength) {
		// Check if we should start breaking
		FVector2D normVelocity = to2D(velocity);
		normVelocity.Normalize();

		FVector2D normDistLeft = target - to2D(agent->GetActorLocation());
		normDistLeft.Normalize();

		if (normVelocity.Equals(normDistLeft, 0.1)) {
			return -newAcceleration;
		}
	}
	
	return newAcceleration;
}

bool ADynamicPointMassController::updateTarget_moving()
{
	FVector2D oldTarget = target;

	// The agent is following a moving formation.

	agent->findAgents();	// Check if we can see any new agents.

	if (agent->numberUnseenAgents() > 0) {
		// We cannot see all the other agents so move towards the agents we can see.
		searching = true;
		target = approachAgents();

	} else {
		// We know where all the other agents are.

		formation->foundAllAgents(agent);	// Tell formation that we have found all agents.

		try {
			target = formation->getTarget(agent);

			// Check if I am moving towards the target right now, else move to old target.
			//if (isMovingTowardsTarget(newTarget)) {
				// Not moving towards the target == we should be stopping.

				//target = newTarget;
				//target = to2D(agent->GetActorLocation());	// Move towards myself.
			//}

		} catch (std::exception e) {
			// At least one of the other agents do not know where everybody else are.
			// I will move towards all the other agents.
			target = approachAgents();
		}
	}

	return target.Equals(oldTarget, 0.001);
}
