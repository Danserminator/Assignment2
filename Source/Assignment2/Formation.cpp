// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "Formation.h"

//#define OUTPUT

// Sets default values
AFormation::AFormation()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFormation::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFormation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	location += velocity * GWorld->GetWorld()->GetDeltaSeconds();

	if (drawFormation) {
		FVector loc;
		FVector formationLoc = FVector(location, 0);
		for (int32 c = 0; c < formationPositions.Num(); c++) {
			loc = FVector(formationPositions[c], 0) + formationLoc;
			DrawDebugLine(GWorld->GetWorld(), loc, loc + formationSize, formationColor, false, 0.1, 0, 1);
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Blue, FString::Printf(TEXT("%s\r\n"), *location.ToString()));
}

// Called to bind functionality to input
void AFormation::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AFormation::initFormation(TArray<FVector2D> positions, FVector velocity)
{
	formationPositions = positions;
	this->velocity = FVector2D(velocity.X, velocity.Y);
}

void AFormation::setNumAgents(int32 n)
{
	numAgents = n;
}

int32 AFormation::foundAllAgents(FVector2D location)
{
	int32 index = agentPositions.Num();

	agentPositions.Add(location);

	if (agentPositions.Num() == numAgents) {
		assignPositions();
	}

	return index;
}

FVector2D AFormation::getTarget(int32 i)
{
	if (agentPositions.Num() == numAgents) {
		return (formationPositions[assignedPositions[i].Y] + location);
	}

	throw std::exception("");
}

void AFormation::assignPositions()
{
	// Move formation origin to center of mass
	moveFormation();

	// Create cost matrix
	TArray<TArray<float>> matrix = createMatrix();

	#ifdef OUTPUT
	UE_LOG(LogTemp, Warning, TEXT("------------MATRIX-------------"));
	for (int32 c = 0; c < matrix.Num(); c++) {
		FString str;
		for (int32 g = 0; g < matrix[c].Num(); g++) {
			str.Append(FString::SanitizeFloat(matrix[c][g]));
			str.Append("\t");
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	}
	UE_LOG(LogTemp, Warning, TEXT("-------------------------------\n\n"));
	#endif

#ifdef OUTPUT
	TArray<TArray<float>> matrixCopy = matrix;
	float cost = 0;
#endif

	// Find cheapest allocation of tasks
	assignedPositions = assignTasks(matrix);

	#ifdef OUTPUT
	for (int32 c = 0; c < assignments.Num(); c++) {
		cost += matrixCopy[assignedPositions[c].X][assignedPositions[c].Y];
	}

	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Cost: %f\r\n"), cost));
	#endif
}

TArray<FVector2D> AFormation::assignTasks(TArray<TArray<float>> & matrix)
{
	// For each element in each row subtract with the smallest element in that row.
	stepOne(matrix);

	// Check if we can assign tasks to all agents now.
	if (canAssign(matrix)) {
		return assign(matrix);
	}

	// For each element in each column subtract with the smallest element in that column.
	stepTwo(matrix);

	// Continue until we can make an assignment.
	while (!canAssign(matrix)) {
#ifdef OUTPUT
		UE_LOG(LogTemp, Warning, TEXT("------------MATRIX-------------"));
		for (int32 c = 0; c < matrix.Num(); c++) {
			FString str;
			for (int32 g = 0; g < matrix[c].Num(); g++) {
				str.Append(FString::SanitizeFloat(matrix[c][g]));
				str.Append("\t");
			}
			UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
		}
		UE_LOG(LogTemp, Warning, TEXT("-------------------------------\n\n"));
#endif
		TArray<TArray<int32>> lines = stepThree(matrix);

#ifdef OUTPUT
		UE_LOG(LogTemp, Warning, TEXT("------------LINES-----------"));
		for (int32 c = 0; c < lines.Num(); c++) {
			FString str;
			for (int32 g = 0; g < lines[c].Num(); g++) {
				str.Append(FString::SanitizeFloat(lines[c][g]));
				str.Append("\t");
			}
			UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
		}
		UE_LOG(LogTemp, Warning, TEXT("-------------------------------\n\n"));
#endif

		stepFour(matrix, lines);
	}

	return assign(matrix);
}

TArray<TArray<int32>> AFormation::stepThree(TArray<TArray<float>> & matrix)
{
	TArray<TArray<int32>> assignmentMatrix;	// Assigned = 1, crossed = 0, neither = -1

	TArray<int32> row;
	for (int32 c = 0; c < matrix[0].Num(); c++) {
		row.Add(-1);
	}
	for (int32 c = 0; c < matrix.Num(); c++) {
		assignmentMatrix.Add(row);
	}

	int32 numAssigned = 0;

	// Assign or cross out every zero element.
	bool done = false;
	bool changed;
	while (!done) {
		done = true;
		changed = false;

		// Assign rows with just one zero element.
		for (int32 c = 0; c < matrix.Num(); c++) {
			int32 numZeros = 0;
			int32 index = 0;
			for (int32 g = 0; g < matrix[c].Num(); g++) {
				if (matrix[c][g] == 0 && assignmentMatrix[c][g] == -1) {
					numZeros++;
					index = g;
					if (numZeros > 1) {
						break;
					}
				}
			}
			if (numZeros == 1) {
				changed = true;
				assignmentMatrix[c][index] = 1;
				for (int32 g = 0; g < matrix[c].Num(); g++) {
					if (g == index) {
						continue;
					}
					if (matrix[c][g] == 0) {
						assignmentMatrix[c][g] = 0;
					}
				}

				for (int32 g = 0; g < matrix.Num(); g++) {
					if (g == c) {
						continue;
					}
					if (matrix[g][index] == 0) {
						assignmentMatrix[g][index] = 0;
					}
				}
			}
		}

		// Assign columns with only one zero element.
		for (int32 g = 0; g < matrix[0].Num(); g++) {
			int32 numZeros = 0;
			int32 index = 0;
			for (int32 c = 0; c < matrix.Num(); c++) {
				if (matrix[c][g] == 0 && assignmentMatrix[c][g] == -1) {
					numZeros++;
					index = c;
					if (numZeros > 1) {
						break;
					}
				}
			}
			if (numZeros == 1) {
				changed = true;
				assignmentMatrix[index][g] = 1;
				for (int32 c = 0; c < matrix.Num(); c++) {
					if (c == index) {
						continue;
					}
					if (matrix[c][g] == 0) {
						assignmentMatrix[c][g] = 0;
					}
				}
				for (int32 c = 0; c < matrix[index].Num(); c++) {
					if (c == g) {
						continue;
					}
					if (matrix[index][c] == 0) {
						assignmentMatrix[index][c] = 0;
					}
				}
			}
		}

		if (changed) {
			done = false;
		} else {
			// Assign rows with multiple zero elements (assigns the first one...).
			for (int32 c = 0; c < matrix.Num(); c++) {
				for (int32 g = 0; g < matrix[c].Num(); g++) {
					if (matrix[c][g] == 0 && assignmentMatrix[c][g] == -1) {
						changed = true;
						assignmentMatrix[c][g] = 1;

						// Cross out all other zeros in this row.
						for (int32 r = g + 1; r < matrix[c].Num(); r++) {
							if (matrix[c][r] == 0) {
								assignmentMatrix[c][r] = 0;
							}
						}
						// Cross out all other zeros in this column.
						for (int32 r = c + 1; r < matrix.Num(); r++) {
							if (matrix[r][g] == 0) {
								assignmentMatrix[r][g] = 0;
							}
						}
					}
				}
			}

			if (changed) {
				done = false;
				continue;
			}

			// Assign columns with multiple zero elements (assigns the first one...).
			for (int32 g = 0; g < matrix[0].Num(); g++) {
				for (int32 c = 0; c < matrix.Num(); c++) {
					if (matrix[c][g] == 0 && assignmentMatrix[c][g] == -1) {
						changed = true;
						assignmentMatrix[c][g] = 1;

						// Cross out all other zeros in this row.
						for (int32 r = g + 1; r < matrix[c].Num(); r++) {
							if (matrix[c][r] == 0) {
								assignmentMatrix[c][r] = 0;
							}
						}
						// Cross out all other zeros in this column.
						for (int32 r = c + 1; r < matrix.Num(); r++) {
							if (matrix[r][g] == 0) {
								assignmentMatrix[r][g] = 0;
							}
						}
					}
				}
			}

			if (changed) {
				done = false;
			}
		}
	}

#ifdef OUTPUT
	UE_LOG(LogTemp, Warning, TEXT("------------ASSIGNMENT-----------"));
	for (int32 c = 0; c < assignmentMatrix.Num(); c++) {
		FString str;
		for (int32 g = 0; g < assignmentMatrix[c].Num(); g++) {
			str.Append(FString::SanitizeFloat(assignmentMatrix[c][g]));
			str.Append("\t");
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	}
	UE_LOG(LogTemp, Warning, TEXT("-------------------------------\n\n"));
#endif

	// There is a solution
	if (numAssigned == matrix.Num()) {
		// TODO: fix so it ends here...
	}

	// Drawing part.
	// Mark all rows having no assignment.
	TArray<bool> markedRows;
	for (int32 c = 0; c < matrix.Num(); c++) {
		bool mark = true;
		for (int32 g = 0; g < matrix[c].Num(); g++) {
			if (assignmentMatrix[c][g] == 1) {
				mark = false;
				break;
			}
		}
		markedRows.Add(mark);
	}

	TArray<bool> markedColumns;
	for (int32 c = 0; c < matrix[0].Num(); c++) {
		markedColumns.Add(false);
	}

	TArray<bool> newlyMarkedRows = markedRows;
	TArray<bool> newlyMarkedColumns = markedColumns;
	changed = true;
	while (changed) {
		changed = false;

		// Mark all (unmarked) columns having zeros in newly marked row(s).
		for (int32 c = 0; c < matrix[0].Num(); c++) {
			newlyMarkedColumns[c] = false;
			// Already marked.
			if (markedColumns[c]) {
				continue;
			}

			// Check if this column have any marked row(s)
			for (int32 g = 0; g < matrix.Num(); g++) {
				if (newlyMarkedRows[g]) {
					if (matrix[g][c] == 0) {	// TODO: might need epsilon?
						markedColumns[c] = true;
						newlyMarkedColumns[c] = true;
						changed = true;
						break;
					}
				}
			}
		}

		// Mark all rows having assignments in newly marked columns.
		for (int32 c = 0; c < matrix.Num(); c++) {
			newlyMarkedRows[c] = false;
			// Already marked.
			if (markedRows[c]) {
				continue;
			}
			
			for (int32 g = 0; g < matrix[c].Num(); g++) {
				if (newlyMarkedColumns[g]) {
					if (assignmentMatrix[c][g] == 1) {
						markedRows[c] = true;
						newlyMarkedRows[c] = true;
						changed = true;
						break;
					}
				}
			}
		}
	}
	
	// Draw lines through all marked columns and unmarked rows.
	TArray<TArray<int32>> lines;	// The value of each element represent how many lines that crosses it.
	for (int32 c = 0; c < matrix.Num(); c++) {
		TArray<int32> temp;
		if (!markedRows[c]) {
			for (int32 g = 0; g < matrix[c].Num(); g++) {
				temp.Add(1);
			}
		} else {
			for (int32 g = 0; g < matrix[c].Num(); g++) {
				temp.Add(0);
			}
		}
		lines.Add(temp);
	}

	for (int32 c = 0; c < matrix[0].Num(); c++) {
		if (markedColumns[c]) {
			for (int32 g = 0; g < matrix.Num(); g++) {
				lines[g][c]++;
			}
		}
	}

#ifdef OUTPUT
	UE_LOG(LogTemp, Warning, TEXT("------------MARKED ROWS-----------"));
	FString str;
	for (int32 g = 0; g < markedRows.Num(); g++) {
		str.Append(FString::SanitizeFloat(markedRows[g]));
		str.Append("\t");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	UE_LOG(LogTemp, Warning, TEXT("-------------------------------\n\n"));

	UE_LOG(LogTemp, Warning, TEXT("------------MARKED COLUMNS-----------"));
	FString str2;
	for (int32 g = 0; g < markedColumns.Num(); g++) {
		str2.Append(FString::SanitizeFloat(markedColumns[g]));
		str2.Append("\t");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *str2);
	UE_LOG(LogTemp, Warning, TEXT("-------------------------------\n\n"));
#endif
	
	/*
	for (int32 c = 0; c < matrix.Num(); c++) {
		TArray<int32> temp;
		for (int32 g = 0; g < matrix[c].Num(); g++) {
			if (markedColumns[g] && !markedRows[c]) {
				temp.Add(2);
			} else if (markedColumns[g] || !markedRows[c]) {
				temp.Add(1);
			} else {
				temp.Add(0);
			}
		}
		lines.Add(temp);
	}
	*/
	return lines;
}

void AFormation::stepFour(TArray<TArray<float>> & matrix, TArray<TArray<int32>> & lines)
{
	// Find the lowest values from the elements that are left (0 in lines).
	// Subtract this element from every unmarked element and add it to every element 
	// covered by two lines.

	// Find lowest value.
	float minVal = std::numeric_limits<float>::infinity();
	for (int32 c = 0; c < lines.Num(); c++) {
		for (int32 g = 0; g < lines[c].Num(); g++) {
			if (lines[c][g] == 0) {
				minVal = (matrix[c][g] < minVal) ? matrix[c][g] : minVal;
			}
		}
	}

	// Subtract and add the lowest value.
	for (int32 c = 0; c < lines.Num(); c++) {
		for (int32 g = 0; g < lines[c].Num(); g++) {
			if (lines[c][g] == 0) {
				matrix[c][g] -= minVal;
			} else if (lines[c][g] == 2) {
				matrix[c][g] += minVal;
			}
		}
	}
}

void AFormation::stepTwo(TArray<TArray<float>> & matrix)
{
	for (int32 c = 0; c < matrix[0].Num(); c++) {
		float minVal = getMinCol(matrix, c);

		for (int32 g = 0; g < matrix.Num(); g++) {
			matrix[g][c] -= minVal;
		}
	}
}

float AFormation::getMinCol(TArray<TArray<float>> & matrix, int32 column)
{
	float minVal = std::numeric_limits<float>::infinity();

	for (int32 c = 0; c < matrix.Num(); c++) {
		minVal = (matrix[c][column] < minVal) ? matrix[c][column] : minVal;
	}

	return minVal;
}

TArray<FVector2D> AFormation::assign(TArray<TArray<float>> & matrix)
{
	TArray<TArray<bool>> assignment;

	TArray<bool> row;
	for (int32 c = 0; c < matrix[0].Num(); c++) {
		row.Add(false);
	}
	for (int32 c = 0; c < matrix.Num(); c++) {
		assignment.Add(row);
	}

	findSolution(matrix, assignment, 0);


	TArray<FVector2D> assignPair;
	for (int32 c = 0; c < assignment.Num(); c++) {
#ifdef OUTPUT
		FString str;
#endif
		for (int32 g = 0; g < assignment[c].Num(); g++) {
			if (assignment[c][g]) {
				assignPair.Add(FVector2D(c, g));
#ifdef OUTPUT
				str.Append("1\t");
			} else {
				str.Append("0\t");
#endif
			}
		}
#ifdef OUTPUT
		UE_LOG(LogTemp, Warning, TEXT("%s\r\n"), *str);
#endif
	}

#ifdef OUTPUT
	for (int32 c = assignment.Num() - 1; c >= 0; c--) {
		FString str;
		for (int32 g = 0; g < assignment[c].Num(); g++) {
			if (assignment[c][g]) {
				str.Append("1   ");
			} else {
				str.Append("0   ");
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("%s\r\n"), *str));
	}
#endif

	return assignPair;
}

bool AFormation::findSolution(TArray<TArray<float>> & matrix, TArray<TArray<bool>> & assignment, int32 row)
{
	if (row >= matrix.Num()) {
		return true;
	}

	for (int32 c = 0; c < matrix[row].Num(); c++) {
		if (matrix[row][c] == 0) {
			// Check so no other in this column in selected already.
			bool alreadyTaken = false;
			for (int32 g = 0; g < row; g++) {
				if (assignment[g][c]) {
					alreadyTaken = true;
					break;
				}
			}

			if (alreadyTaken) {
				continue;
			}

			assignment[row][c] = true;

			if (findSolution(matrix, assignment, row + 1)) {
				return true;
			} else {
				assignment[row][c] = false;
			}
		}
	}

	return false;
}

bool AFormation::canAssign(TArray<TArray<float>> & matrix)
{
	TArray<TArray<bool>> assignment;

	TArray<bool> row;
	for (int32 c = 0; c < matrix[0].Num(); c++) {
		row.Add(false);
	}
	for (int32 c = 0; c < matrix.Num(); c++) {
		assignment.Add(row);
	}

	return findSolution(matrix, assignment, 0);
}

void AFormation::stepOne(TArray<TArray<float>> & matrix)
{
	for (int32 c = 0; c < matrix.Num(); c++) {
		float minVal = getMin(matrix[c]);

		for (int32 g = 0; g < matrix[c].Num(); g++) {
			matrix[c][g] -= minVal;
		}
	}
}

float AFormation::getMin(TArray<float> arr) {
	float minVal = std::numeric_limits<float>::infinity();

	for (int32 c = 0; c < arr.Num(); c++) {
		minVal = (arr[c] < minVal) ? arr[c] : minVal;
	}

	return minVal;
}

void AFormation::moveFormation()
{
	float yMin = std::numeric_limits<float>::infinity();
	float yMax = std::numeric_limits<float>::min();
	float xMin = std::numeric_limits<float>::infinity();
	float xMax = std::numeric_limits<float>::min(); 

	// Find spread of agents
	FVector2D loc;
	for (int32 c = 0; c < agentPositions.Num(); c++) {
		loc = agentPositions[c];

		if (yMax < loc.Y) yMax = loc.Y;
		if (yMin > loc.Y) yMin = loc.Y;
		if (xMax < loc.X) xMax = loc.X;
		if (xMin > loc.X) xMin = loc.X;
	}

	// Move formation to "center of mass"
	float yPos = ((yMax - yMin) / 2) + yMin;
	float xPos = ((xMax - xMin) / 2) + xMin;
	location = FVector2D(xPos, yPos);

	drawFormation = true;
}

TArray<TArray<float>> AFormation::createMatrix()
{
	TArray<TArray<float>> matrix;

	for (int32 c = 0; c < agentPositions.Num(); c++) {
		TArray<float> agentCosts;

		#ifdef OUTPUT
		FString str;
		#endif

		for (int32 g = 0; g < formationPositions.Num(); g++) {
			float cost = costHeuristic(agentPositions[c], (formationPositions[g] + location));
			agentCosts.Add(cost);

			#ifdef OUTPUT
			str.Append(FString::SanitizeFloat(cost));
			str.Append("\t");
			#endif
		}

		#ifdef OUTPUT
		FVector loc = agents[c]->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("Agent (%f, %f): %s\r\n"), loc.X, loc.Y, *str);
		#endif

		matrix.Add(agentCosts);
	}

	return matrix;
}

float AFormation::costHeuristic(FVector2D agent, FVector2D goal)
{
	float cost = FVector2D::Distance(agent, goal);
	return cost * cost;
}

/*

TArray<FVector2D> AFormation::assignTasks(TArray<TArray<float>> matrix)
{
	// Generate mask matrix and arrays for "covered" rows and columns
	TArray<bool> rowCover, colCover;
	TArray<TArray<int32>>  maskMatrix;
	for (int32 c = 0; c < matrix.Num(); c++) {
		rowCover.Add(false);
		colCover.Add(false);

		TArray<int32> maskRow;
		for (int32 g = 0; g < matrix[c].Num(); g++) {
			maskRow.Add(0);
		}
		maskMatrix.Add(maskRow);
	}

	int32 step = 1;
	int32 path_row_0, path_col_0;			// ?????
	bool done = false;
	while (!done) {
		switch (step) {
			case 1:
				step = step_one(matrix);
				UE_LOG(LogTemp, Warning, TEXT("STEP ONE DONE!\r\n"));
				break;
			case 2:
				step = step_two(matrix, maskMatrix, rowCover, colCover);
				UE_LOG(LogTemp, Warning, TEXT("STEP TWO DONE!\r\n"));
				break;
			case 3:
				step = step_three(maskMatrix, colCover);
				UE_LOG(LogTemp, Warning, TEXT("STEP THREE DONE!\r\n"));
				break;
			case 4:
				step = step_four(matrix, maskMatrix, rowCover, colCover, path_row_0, path_col_0);
				UE_LOG(LogTemp, Warning, TEXT("STEP FOUR DONE!\r\n"));
				break;
			case 5:
				step = step_five(maskMatrix, rowCover, colCover, path_row_0, path_col_0);
				UE_LOG(LogTemp, Warning, TEXT("STEP FIVE DONE!\r\n"));
				break;
			case 6:
				step = step_six(matrix, rowCover, colCover);
				UE_LOG(LogTemp, Warning, TEXT("STEP SIX DONE!\r\n"));
				break;
			case 7:
				done = true;
				break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("MASK MATRIX!\r\n"));

	TArray<FVector2D> pairs;
	for (int32 c = 0; c < maskMatrix.Num(); c++) {

		#ifdef OUTPUT
		FString str, str2;
		#endif

		for (int32 g = 0; g < maskMatrix[c].Num(); g++) {
			if (maskMatrix[c][g] == 1) {
				pairs.Add(FVector2D(c, g));

				#ifdef OUTPUT
				str.Append("1\t");
				str2.InsertAt(0, "1   ");
			} else {
				str.Append("0\t");
				str2.InsertAt(0, "0   ");
				#endif
			}
		}

		#ifdef OUTPUT
		UE_LOG(LogTemp, Warning, TEXT("%s\r\n"), *str);
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("%s\r\n"), *str2));
		#endif
	}

	return pairs;
}

int32 AFormation::step_one(TArray<TArray<float>> & matrix)
{
	// Subtract smallest entry in each row
	float min_in_row;

	for (int32 c = 0; c < matrix.Num(); c++) {
		min_in_row = std::numeric_limits<float>::infinity();

		for (int32 g = 0; g < matrix[c].Num(); g++) {
			if (matrix[c][g] < min_in_row) {
				min_in_row = matrix[c][g];
			}
		}

		for (int32 g = 0; g < matrix[c].Num(); g++) {
			matrix[c][g] -= min_in_row;
		}
	}

	return 2;
}

int32 AFormation::step_two(TArray<TArray<float>> & matrix,
						   TArray<TArray<int32>> & maskMatrix, 
						   TArray<bool> & rowCover, TArray<bool> & colCover)
{
	// Find a 0 (Z) in resulting matrix. Star Z if no starred 0 in its row or column
	for (int32 c = 0; c < matrix.Num(); c++) {
		for (int32 g = 0; g < matrix[c].Num(); g++) {
			if (matrix[c][g] == 0 && !rowCover[c] && !colCover[g]) {
				maskMatrix[c][g] = 1;
				rowCover[c] = true;
				colCover[g] = true;
			}
		}
	}

	for (int32 c = 0; c < matrix.Num(); c++) {
		rowCover[c] = false;
		colCover[c] = false;
	}

	return 3;
}

int32 AFormation::step_three(TArray<TArray<int32>> & maskMatrix, TArray<bool> & colCover)
{
	// Cover each column containing a starred zero.
	// If K columns are covered, the starred zeros describe a complete set of unique assignments
	for (int32 c = 0; c < maskMatrix.Num(); c++) {
		for (int32 g = 0; g < maskMatrix[c].Num(); g++) {
			if (maskMatrix[c][g] == 1) {
				colCover[g] = true;
			}
		}
	}
	
	// Count covered columns
	int32 colCount = 0;
	for (int32 c = 0; c < colCover.Num(); c++) {
		if (colCover[c]) colCount++;
	}

	if (colCount >= colCover.Num()) return 7;
	else							return 4;
}

int32 AFormation::step_four(TArray<TArray<float>> & matrix,
							TArray<TArray<int32>> & maskMatrix,
							TArray<bool> & rowCover, TArray<bool> & colCover,
							int32 & path_row_0, int32 & path_col_0)
{
	// Find a noncovered zero and prime it.
	// If there is no starred zero in the row contining this primed zero, go to step 5.
	// Otherwise, cover this row and uncover the columnt containing starred zero.
		// Continue in this manner until there are no uncovered zeros left. 
		// Save the smallest uncovered value and go to step 6. (???)
	int32 row = -1;
	int32 col = -1;

	while (true) {
		find_a_zero(matrix, rowCover, colCover, row, col);

		if (row == -1) {
			return 6;
		} else {
			maskMatrix[row][col] = 2;
			if (star_in_row(maskMatrix, row)) {
				find_star_in_row(maskMatrix, row, col);
				rowCover[row] = true;
				colCover[col] = false;
			} else {
				path_row_0 = row;
				path_col_0 = col;
				return 5;
			}
		}
	}
}

void AFormation::find_a_zero(TArray<TArray<float>> & matrix,
							 TArray<bool> & rowCover, TArray<bool> & colCover,
							 int32 & row, int32 & col)
{
	int32 r = 0;
	int32 c;
	bool done = false;
	row = -1;
	col = -1;
	
	while (!done) {
		c = 0;

		while (true) {
			//UE_LOG(LogTemp, Warning, TEXT("matrix[%d][%d] == %f && rowCover[%d] == %d && colCover[%d] == %d\r\n"), r, c, matrix[r][c], r, rowCover[r], c, colCover[c]);
			if (matrix[r][c] == 0 && !rowCover[r] && !colCover[c]) {
				//UE_LOG(LogTemp, Warning, TEXT("---------------------------------------------"));
				row = r;
				col = c;
				done = true;
			}

			c++;
			if (c >= matrix.Num() || done) break;
		}

		r++;
		if (r >= matrix.Num()) done = true;
	}
}

bool AFormation::star_in_row(TArray<TArray<int32>> & maskMatrix, int32 row)
{
	bool tmp = false;
	for (int32 c = 0; c < maskMatrix[row].Num(); c++) {
		if (maskMatrix[row][c] == 1) {
			tmp = true;
		}
	}

	return tmp;
}

void AFormation::find_star_in_row(TArray<TArray<int32>> & maskMatrix, int32 row, int32 & col)
{
	col = -1;
	for (int c = 0; c < maskMatrix[row].Num(); c++) {
		if (maskMatrix[row][c] == 1) {
			col = c;
			// TODO: return?
		}
	}
}

int32 AFormation::step_five(TArray<TArray<int32>> & maskMatrix,
							TArray<bool> & rowCover, TArray<bool> & colCover,
							int32 & path_row_0, int32 & path_col_0)
{
	bool done = false;
	int32 r = -1;
	int32 c = -1;

	int32 path_count = 1;
	TArray<FVector2D> path;
	path.Add(FVector2D(path_row_0, path_col_0));

	while (!done) {
		find_star_in_col(maskMatrix, r, int32(path[path_count - 1].Y));
		if (r > -1) {
			path_count++;
			path.Add(FVector2D(r, path[path_count - 2].Y));
		} else {
			done = true;
		}

		if (!done) {
			find_prime_in_row(maskMatrix, int32(path[path_count - 1].X), c);
			path_count++;
			path.Add(FVector2D(path[path_count - 2].X, c));
		}
	}

	augment_path(maskMatrix, path);
	clear_covers(rowCover, colCover);
	erase_primes(maskMatrix);
	return 3;
}

void AFormation::find_star_in_col(TArray<TArray<int32>> & maskMatrix, int32 & row, int32 col)
{
	row = -1;
	for (int32 c = 0; c < maskMatrix.Num(); c++) {
		if (maskMatrix[c][col] == 1) {
			row = c;
			// TODO: return?
		}
	}
}

void AFormation::find_prime_in_row(TArray<TArray<int32>> & maskMatrix, int32 row, int32 & col)
{
	for (int32 c = 0; c < maskMatrix[0].Num(); c++) {
		if (maskMatrix[row][c]) {
			col = c;
			// TODO: return?
		}
	}
}

void AFormation::augment_path(TArray<TArray<int32>> & maskMatrix, TArray<FVector2D> & path)
{
	for (int32 c = 0; c < path.Num(); c++) {
		if (maskMatrix[path[c].X][path[c].Y] == 1) {
			maskMatrix[path[c].X][path[c].Y] = 0;
		} else {
			maskMatrix[path[c].X][path[c].Y] = 1;
		}
	}
}

void AFormation::clear_covers(TArray<bool> & rowCover, TArray<bool> & colCover)
{
	for (int32 c = 0; c < rowCover.Num(); c++) {
		rowCover[c] = false;
		colCover[c] = false;
	}
}

void AFormation::erase_primes(TArray<TArray<int32>> & maskMatrix)
{
	for (int32 c = 0; c < maskMatrix.Num(); c++) {
		for (int32 g = 0; g < maskMatrix[c].Num(); g++) {
			if (maskMatrix[c][g] == 2) {
				maskMatrix[c][g] = 0;
			}
		}
	}
}

int32 AFormation::step_six(TArray<TArray<float>> & matrix,
						   TArray<bool> & rowCover, TArray<bool> & colCover)
{
	float minVal = std::numeric_limits<float>::infinity();

	find_smallest(matrix, rowCover, colCover, minVal);

	for (int32 c = 0; c < matrix.Num(); c++) {
		for (int32 g = 0; g < matrix[c].Num(); g++) {
			if (rowCover[c]) {
				matrix[c][g] += minVal;
			}
			if (!colCover[g]) {
				matrix[c][g] -= minVal;
			}
		}
	}

	return 4;
}

void AFormation::find_smallest(TArray<TArray<float>>& matrix, TArray<bool>& rowCover, TArray<bool>& colCover, float & minVal)
{
	for (int32 c = 0; c < matrix.Num(); c++) {
		for (int32 g = 0; g < matrix[c].Num(); g++) {
			if (!rowCover[c] && !colCover[g]) {
				if (minVal > matrix[c][g]) {
					minVal = matrix[c][g];
				}
			}
		}
	}
}
*/

/*
TArray<FVector2D> AFormation::assignTasks(TArray<TArray<float>> matrix)
{
	// Subtract smallest entry in each row
	for (int32 c = 0; c < matrix.Num(); c++) {
		float min = getMinRow(matrix[c]);

		for (int32 g = 0; g < matrix[c].Num(); g++) {
			matrix[c][g] -= min;
		}
	}

	// Subtract smallest entry in each column
	for (int32 c = 0; c < matrix[0].Num(); c++) {
		float min = getMinCol(matrix, c);

		for (int32 g = 0; g < matrix.Num(); g++) {
			matrix[g][c] -= min;
		}
	}



	return TArray<FVector2D>();
}

float AFormation::getMinRow(TArray<float> row) {
	float minVal = std::numeric_limits<float>::infinity();

	for (int32 c = 0; c < row.Num(); c++) {
		if (row[c] < minVal) {
			minVal = row[c];
		}
	}

	return minVal;
}

float AFormation::getMinCol(TArray<TArray<float>> matrix, int32 column)
{
	float minVal = std::numeric_limits<float>::infinity();

	for (int32 c = 0; c < matrix.Num(); c++) {
		if (matrix[c][column] < minVal) {
			minVal = matrix[c][column];
		}
	}

	return minVal;
}
*/