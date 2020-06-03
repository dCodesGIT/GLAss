// To demonstrate Relational operators
// Date : 29 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int D_a, D_b;
	int D_result;

	//code
	printf("\n Enter one integer, A : ");
	scanf("%d", &D_a);
	printf("\n Enter another integer, B : ");
	scanf("%d", &D_b);

	printf("\n\n If result is 0, it is FALSE.");
	printf("\n If result is 1, it is TRUE.");

	D_result = (D_a < D_b);
	printf("\n\n (a < b) -> A = %d is LESS THAN B = %d", D_a, D_b);
	printf("\n Result = %d", D_result);

	D_result = (D_a > D_b);
	printf("\n\n (a > b) -> A = %d is GREATER THAN B = %d", D_a, D_b);
	printf("\n Result = %d", D_result);

	D_result = (D_a <= D_b);
	printf("\n\n (a <= b) -> A = %d is LESS THAN OR EQUAL to B = %d", D_a, D_b);
	printf("\n Result = %d", D_result);

	D_result = (D_a >= D_b);
	printf("\n\n (a >= b) -> A = %d is GREATER THAN OR EQUAL to B = %d", D_a, D_b);
	printf("\n Result = %d", D_result);

	D_result = (D_a == D_b);
	printf("\n\n (a == b) -> A = %d is EQUAL to B = %d", D_a, D_b);
	printf("\n Result = %d", D_result);

	D_result = (D_a != D_b);
	printf("\n\n (a != b) -> A = %d is NOT EQUAL to  B = %d", D_a, D_b);
	printf("\n Result = %d", D_result);

	printf("\n\n");
	return 0;
}
