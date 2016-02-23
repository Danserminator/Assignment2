// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Agent.h"
#include <limits>
#include "Engine.h"
#include "Formation.generated.h"

UCLASS()
class ASSIGNMENT2_API AFormation : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFormation();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Stationary Formation")
	void assignPositions(TArray<AAgent *> agents, TArray<FVector2D> positions);

private:
	void adjustPositions(TArray<AAgent *> & agents, TArray<FVector2D> & positions);

	TArray<TArray<float>> createMatrix(TArray<AAgent *> agents, TArray<FVector2D> positions);

	TArray<FVector2D> assignTasks(TArray<TArray<float>> & smatrix);

	void stepOne(TArray<TArray<float>> & matrix);

	float getMin(TArray<float> arr);

	bool canAssign(TArray<TArray<float>> & matrix);

	void stepTwo(TArray<TArray<float>> & matrix);

	float getMinCol(TArray<TArray<float>> & matrix, int32 column);

	TArray<TArray<int32>> stepThree(TArray<TArray<float>> & matrix);

	void stepFour(TArray<TArray<float>> & matrix, TArray<TArray<int32>> & lines);

	TArray<FVector2D> assign(TArray<TArray<float>> & matrix);

	bool findSolution(TArray<TArray<float>> & matrix, TArray<TArray<bool>> & assignment, int32 row);

	
	float costHeuristic(FVector agent, FVector2D goal);

	/*
	TArray<FVector2D> assignTasks(TArray<TArray<float>> matrix);

	int32 step_one(TArray<TArray<float>> & matrix);

	int32 step_two(TArray<TArray<float>> & matrix, TArray<TArray<int32>> & maskMatrix,
				   TArray<bool> & rowCover, TArray<bool> & colCover);

	int32 step_three(TArray<TArray<int32>> & maskMatrix, TArray<bool> & colCover);

	int32 step_four(TArray<TArray<float>> & matrix,
					TArray<TArray<int32>> & maskMatrix,
					TArray<bool> & rowCover, TArray<bool> & colCover,
					int32 & path_row_0, int32 & path_col_0);

	void find_a_zero(TArray<TArray<float>> & matrix,
					 TArray<bool> & rowCover, TArray<bool> & colCover,
					 int32 & row, int32 & col);

	bool star_in_row(TArray<TArray<int32>> & maskMatrix, int32 row);

	void find_star_in_row(TArray<TArray<int32>> & maskMatrix, int32 row, int32 & col);

	int32 step_five(TArray<TArray<int32>> & maskMatrix,
					TArray<bool> & rowCover, TArray<bool> & colCover,
					int32 & path_row_0, int32 & path_col_0);

	void find_star_in_col(TArray<TArray<int32>> & maskMatrix,
						  int32 & row, int32 col);

	void find_prime_in_row(TArray<TArray<int32>> & maskMatrix,
						   int32 row, int32 & col);

	void augment_path(TArray<TArray<int32>> & maskMatrix, TArray<FVector2D> & path);

	void clear_covers(TArray<bool> & rowCover, TArray<bool> & colCover);

	void erase_primes(TArray<TArray<int32>> & maskMatrix);

	int32 step_six(TArray<TArray<float>> & matrix,
				   TArray<bool> & rowCover, TArray<bool> & colCover);

	void find_smallest(TArray<TArray<float>> & matrix,
					   TArray<bool> & rowCover, TArray<bool> & colCover,
					   float & minVal);

	*/
	//float getMinRow(TArray<float> row);

	//float getMinCol(TArray<TArray<float>> matrix, int32 column);
	
};
