// To demonstrate pointers - Array of structure using pointers
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#define NAME_LENGTH 100

struct Employee {
	char name[NAME_LENGTH];
	int age;
	float salary;
	char sex;
	char marital_status;
};

int main() {
	// Function declaration
	void GetName(char *, int);

	// Variable declaration
	int i, nEmps;
	struct Employee *pEmpRecords = NULL;

	// Code
	printf("\n How many employees are there : ");
	scanf("%d", &nEmps);

	pEmpRecords = (struct Employee *)malloc(sizeof(struct Employee) * nEmps);
	if(pEmpRecords == NULL) {
		printf("\n Memory allocation failed!!!");
		exit(0);
	}

	for(i = 0; i < nEmps; i++) {
		printf("\n\n Employee number : %d", i + 1);
		printf("\n Name : ");
		GetName(pEmpRecords[i].name, NAME_LENGTH);
		printf("\n Age : ");
		scanf("%d", &pEmpRecords[i].age);
		printf(" Sex (M/F) : ");
		pEmpRecords[i].sex = getche();
		pEmpRecords[i].sex = toupper(pEmpRecords[i].sex);
		printf("\n Salary : ");
		scanf("%f", &pEmpRecords[i].salary);
		printf(" Is employee married? (Y/N) : ");
		pEmpRecords[i].marital_status = getche();
		pEmpRecords[i].marital_status = toupper(pEmpRecords[i].marital_status);
	}

	printf("\n\n");
	printf("\n *** Displaying Records of employee ***");
	for(i = 0; i < nEmps; i++) {
		printf("\n\n Employee number : %d", i + 1);
		printf("\n Name : %s", pEmpRecords[i].name);
		printf("\n Age : %d", pEmpRecords[i].age);
		printf("\n Gender : ");
		if(pEmpRecords[i].sex == 'M')
			printf("Male");
		else if(pEmpRecords[i].sex == 'F')
			printf("Female");
		else
			printf("Invalid");
		printf("\n Salary : Rs. %f", pEmpRecords[i].salary);
		printf("\n Marital status : ");
		if(pEmpRecords[i].marital_status == 'Y')
			printf("Married");
		else
			printf("Unmarried");
	}

	if(pEmpRecords) {
		free(pEmpRecords);
		pEmpRecords = NULL;
	}

	printf("\n\n");
	return 0;
}

// Function definition
void GetName(char *str, int len) {
	// Variable declaration
	int i;
	char ch = '\0';

	// Code
	i = 0;
	do {
		ch = getche();
		str[i++] = ch;
	} while(ch != '\r' && i < len);
	if(i == len)
		str[i - 1] = '\0';
	else
		str[i] = '\0';
}