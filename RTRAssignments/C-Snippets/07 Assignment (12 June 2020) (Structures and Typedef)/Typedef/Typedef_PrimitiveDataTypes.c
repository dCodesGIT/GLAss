// To demonstrate typedef on primitive datatypes
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>
typedef int MY_INT;		// Global typedef

int main() {
	// Function declaration
	MY_INT add(MY_INT, MY_INT);

	// Typedefs
	typedef int MY_INT;
	typedef float MY_F;
	typedef char CH;
	typedef double DF;
	typedef unsigned int UINT;
	typedef UINT HANDLE;
	typedef HANDLE HWND;
	typedef HANDLE HINSTANCE;

	// Variable declaration
	MY_INT a = 10, i;
	MY_INT Array[] = { 10,20,30,40,50,60,70,80,90,100 };
	MY_F fl = 26.08f;
	const MY_F pi = 3.1428f;
	CH c = 'D';
	CH str_1[] = "Hello";
	CH str_2[][10] = { "RTR","batch","2020-21" };
	DF d = 36.102000;
	UINT uint = 1234;
	HANDLE handle = 456;
	HWND hwnd = 789;
	HINSTANCE hInstance = 987;

	// Code
	printf("\n Type MY_INT variable a = %d", a);
	printf("\n Type MY_INT array is");
	for(i = 0; i < (sizeof(Array) / sizeof(int)); i++)
		printf("\n\t Array[%d] = %d", i, Array[i]);
	printf("\n Type MY_F variable fl = %f", fl);
	printf("\n Type MY_F constant variable pi = %f", pi);
	printf("\n Type DF variable d = %lf", d);
	printf("\n Type CH variable c = %c", c);
	printf("\n Type CH array variable str_1 = %s", str_1);
	printf("\n Type CH 2D array variable str_2");
	for(i = 0; i < (sizeof(str_2) / sizeof(str_2[0])); i++)
		printf("\n\t str_2[%d] = %s", i + 1, str_2[i]);
	printf("\n Type UINT variable uint = %u", uint);
	printf("\n Type HANDLE variable handle = %u", handle);
	printf("\n Type HWND variable hwnd = %u", hwnd);
	printf("\n Type HINSTANCE variable hInstance = %u", hInstance);

	MY_INT x = 90;
	MY_INT y = 30;
	MY_INT ret;
	ret = add(x, y);
	printf("\n Sum of %d and %d is %d", x, y, ret);

	printf("\n\n");
	return 0;
}

MY_INT add(MY_INT a, MY_INT b) {
	// Code
	return(a + b);
}