// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "DynamicPointMassController.h"

//#define OUTPUT

ADynamicPointMassController::ADynamicPointMassController() {

}

// Called when the game starts or when spawned
void ADynamicPointMassController::BeginPlay() 
{
	agent = static_cast<AAgent *>(GetPawn());	// Check if can be set in constructor.

	R = agent->R;
	formation = agent->formation;
	unseenAgents = agent->unseenAgents;
}


// Called every frame
void ADynamicPointMassController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector aCopy;
	if (play) {
		#ifdef OUTPUT
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Hej")));
		#endif

		setTarget();

		if (waypointReached()) {
			// TODO
		} else {
			acceleration = getAcceleration();

			drawLine(5 * acceleration, accelerationColor);

			float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

			acceleration *= deltaSec;

			velocity += acceleration;

			velocity = velocity.GetClampedToSize(-vMax, vMax);

			FVector currentLocation = agent->GetActorLocation();

			FVector newLocation = currentLocation + (velocity * deltaSec);

			setRotation();

			agent->SetActorLocation(newLocation);
			//agent->SetActorLocationAndRotation(newLocation, rotation);
		}
	}

	drawLine(5 * velocity, velocityColor);
}

bool ADynamicPointMassController::waypointReached()
{
	if (AModelController::waypointReached()) {
		float deltaSec = GWorld->GetWorld()->GetDeltaSeconds();

		FVector2D frameAcceleration = to2D(getAcceleration()) * deltaSec;

		FVector2D frameVelocity = to2D(velocity) * deltaSec;

		if (frameVelocity.X > frameAcceleration.X || frameVelocity.Y > frameAcceleration.Y) {
			// Too high velocity for us to stop in this time frame.
			return false;
		} else {
			// Can stop in this time frame.
			velocity = FVector(0, 0, 0);
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

	float distLeftLength = UKismetMathLibrary::VSize(to3D(target) - to3D(to2D(agent->GetActorLocation()))) - safetyBuffer;


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