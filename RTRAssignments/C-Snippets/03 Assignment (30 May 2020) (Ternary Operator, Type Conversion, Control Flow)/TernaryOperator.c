// To demonstrate ternary operators
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int a, b, p, q;
	int i_ans_1, i_ans_2;
	char ch_ans_1, ch_ans_2;

	// Code
	printf("\n Enter an integer, A: ");
	scanf("%d", &a);
	printf("\n Enter another integer, B : ");
	scanf("%d", &b);

	ch_ans_1 = (a > b) ? 'A' : 'B';
	i_ans_1 = (a > b) ? a : b;
	printf("\n Result of ternary operation \'(A > B) ? A : B\' is %c = %d", ch_ans_1, i_ans_1);

	printf("\n\n Now you need to enter two more values.");
	printf("\n Enter an integer, P: ");
	scanf("%d", &p);
	printf("\n Enter another integer, Q : ");
	scanf("%d", &q);

	ch_ans_2 = (p != q) ? 'P' : 'Q';
	i_ans_2 = (p != q) ? p : q;
	printf("\n Result of ternary operation \'(P != Q) ? P : Q\' is %c = %d", ch_ans_2, i_ans_2);

	printf("\n\n");
	return 0;
}