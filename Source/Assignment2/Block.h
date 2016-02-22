// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class ASSIGNMENT2_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:
	void changeScale(float distance);
	
};
