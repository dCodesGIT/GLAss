// To demonstrate structures - array of structures
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<string.h>

#define NAME_LENGTH 25
#define MARITAL_STATUS 10

struct Employee {
	char name[NAME_LENGTH];
	int age;
	float salary;
	char sex;
	char marital_status[MARITAL_STATUS];
};

int main() {
	// Variable declaration
	struct Employee EmployeeRecord[5];
	int i;
	char emp_Darshan[] = "Darshan";
	char emp_Saket[] = "Saket";
	char emp_Vaishnavi[] = "Vaishnavi";
	char emp_Swaraj[] = "Swaraj";
	char emp_Nishant[] = "Nishant";

	// Code
	strcpy(EmployeeRecord[0].name, emp_Darshan);
	EmployeeRecord[0].age = 20;
	EmployeeRecord[0].sex = 'M';
	EmployeeRecord[0].salary = 5000;
	strcpy(EmployeeRecord[0].marital_status, "Unmarried");

	strcpy(EmployeeRecord[1].name, emp_Saket);
	EmployeeRecord[1].age = 25;
	EmployeeRecord[1].sex = 'M';
	EmployeeRecord[1].salary = 25000;
	strcpy(EmployeeRecord[1].marital_status, "Unmarried");

	strcpy(EmployeeRecord[2].name, emp_Vaishnavi);
	EmployeeRecord[2].age = 20;
	EmployeeRecord[2].sex = 'F';
	EmployeeRecord[2].salary = 50000;
	strcpy(EmployeeRecord[2].marital_status, "Unmarried");

	strcpy(EmployeeRecord[3].name, emp_Swaraj);
	EmployeeRecord[3].age = 21;
	EmployeeRecord[3].sex = 'M';
	EmployeeRecord[3].salary = 25000;
	strcpy(EmployeeRecord[3].marital_status, "Married");

	strcpy(EmployeeRecord[4].name, emp_Nishant);
	EmployeeRecord[4].age = 23;
	EmployeeRecord[4].sex = 'M';
	EmployeeRecord[4].salary = 5000;
	strcpy(EmployeeRecord[4].marital_status, "Married");

	printf("\n Displaying employee records...");
	for(i = 0; i < 5; i++) {
		printf("\n\n Employee Number : %d", i+1);
		printf("\n Name : %s", EmployeeRecord[i].name);
		printf("\n Age : %d", EmployeeRecord[i].age);
		printf("\n Sex : ");
		if(EmployeeRecord[i].sex == 'M' || EmployeeRecord[i].sex == 'm')
			printf("Male");
		else
			printf("Female");
		printf("\n Salary : %f", EmployeeRecord[i].salary);
		printf("\n Marital Status : %s", EmployeeRecord[i].marital_status);
	}

	printf("\n\n");
	return 0;
}