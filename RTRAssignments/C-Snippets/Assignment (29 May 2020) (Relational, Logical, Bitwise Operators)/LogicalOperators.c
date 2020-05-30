// To demonstrate Logical operators
// Date : 29 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int D_a, D_b,D_c;
	int D_result;

	//code
	printf("\n Enter one integer, A : ");
	scanf("%d", &D_a);
	printf("\n Enter another integer, B : ");
	scanf("%d", &D_b);
	printf("\n Enter one more integer, C : ");
	scanf("%d", &D_c);

	printf("\n\n If result is 0, it is FALSE.");
	printf("\n If result is 1, it is TRUE.");

	D_result = (D_a <= D_b) && (D_b != D_c);
	printf("\n\n Logical AND (&&) : Answer is TRUE (1), if and only if both the conditions are true.");
	printf("\n \t The answer is FALSE (0), if any one or both the conditions are false.");
	printf("\n A = %d is LESS THAN OR EQUAL to B = %d AND B = %d is NOT EQUAL to C = %d", D_a, D_b, D_b, D_c);
	printf("\n Result = %d", D_result);

	D_result = (D_b >= D_a) || (D_a == D_c);
	printf("\n\n Logical OR (||) : Answer is FALSE (0), if and only if both the conditions are false.");
	printf("\n \t The answer is TRUE (1), if any one or both the conditions are true.");
	printf("\n B = %d is GREATER THAN OR EQUAL to A = %d OR A = %d is EQUAL to C = %d", D_b, D_a, D_a, D_c);
	printf("\n Result = %d", D_result);

	printf("\n\n Logical NOT (!) : Answer is TRUE (1), if the value of variable is zero.");
	printf("\n \t The answer is FALSE (0), if the value odf variable is nonzero.");
	
	D_result = !D_a;
	printf("\n A = %d on logical NOT gives ", D_a);
	printf("\n Result = %d", D_result);
	D_result = !D_b;
	printf("\n B = %d on logical NOT gives ", D_b);
	printf("\n Result = %d", D_result);
	D_result = !D_c;
	printf("\n C = %d on logical NOT gives ", D_c);
	printf("\n Result = %d", D_result);

	D_result = (!(D_a <= D_b) && !(D_b != D_c));
	printf("\n\n Using of logical NOT on (A <= B) and also on (B != C) and then ANDing logically gives");
	printf("\n Result = %d", D_result);

	D_result = !((D_b >= D_a) || (D_a == D_c));
	printf("\n\n Using of logical NOT on entire logical operation (B >= A) || (A == C) gives");
	printf("\n Result = %d", D_result);

	printf("\n\n");
	return 0;
}
