// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2.h"
#include "Formation.h"

#define OUTPUT

// Sets default values
AFormation::AFormation()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorLocation(FVector(0, 0, 0));

}

// Called when the game starts or when spawned
void AFormation::BeginPlay()
{
	Super::BeginPlay();

	TArray<AAgent *> agents;
	TArray<FVector2D> positions;
	assignPositions(agents, positions);
}

// Called every frame
void AFormation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFormation::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AFormation::assignPositions(TArray<AAgent *> agents, TArray<FVector2D> positions)
{
	//TArray<TArray<float>> matrix = createMatrix(agents, positions);

	TArray<TArray<float>> matrix;
	TArray<float> row;

	/*
	row = TArray<float>();
	row.Add(1); row.Add(2); row.Add(3); row.Add(4);
	matrix.Add(row);
	row = TArray<float>();
	row.Add(2); row.Add(4); row.Add(6); row.Add(8);
	matrix.Add(row);
	row = TArray<float>();
	row.Add(3); row.Add(6); row.Add(9); row.Add(12);
	matrix.Add(row);
	row = TArray<float>();
	row.Add(4); row.Add(8); row.Add(12); row.Add(16);
	matrix.Add(row);
	*/
	
	row = TArray<float>();
	row.Add(90); row.Add(75); row.Add(75); row.Add(80);
	matrix.Add(row);
	row = TArray<float>();
	row.Add(35); row.Add(85); row.Add(55); row.Add(65);
	matrix.Add(row);
	row = TArray<float>();
	row.Add(125); row.Add(95); row.Add(90); row.Add(105);
	matrix.Add(row);
	row = TArray<float>();
	row.Add(45); row.Add(110); row.Add(95); row.Add(115);
	matrix.Add(row);

	TArray<FVector2D> agentTaskPair = assignTasks(matrix);

	UE_LOG(LogTemp, Warning, TEXT(""));
}

TArray<TArray<float>> AFormation::createMatrix(TArray<AAgent *> agents, TArray<FVector2D> positions)
{
	TArray<TArray<float>> matrix;

	for (int32 c = 0; c < agents.Num(); c++) {
		TArray<float> agentCosts;

		#ifdef OUTPUT
		FString str;
		#endif

		for (int32 g = 0; g < positions.Num(); g++) {
			float cost = costHeuristic(agents[c]->GetActorLocation(), positions[g]);
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

float AFormation::costHeuristic(FVector agent, FVector2D goal)
{
	return FVector2D::Distance(FVector2D(agent.X, agent.Y), goal);
}

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