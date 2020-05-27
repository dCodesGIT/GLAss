// Understanding of primitive data types and its sizes
#include <stdio.h>
#define MYMACRO 10

int main() {
	//code
    enum myEnum {
        Darshan
    };
    printf("\n Size of 'int' = %ld bytes", sizeof(int));
    printf("\n Size of 'unsigned int' = %ld bytes", sizeof(unsigned int));
    printf("\n Size of 'long' = %ld bytes", sizeof(long));
    printf("\n Size of 'long long' = %ld bytes", sizeof(long long));
    printf("\n Size of 'float' = %ld bytes", sizeof(float));
    printf("\n Size of 'double' = %ld bytes", sizeof(double));
    printf("\n Size of 'long double' = %ld bytes", sizeof(long double));
    //printf("\n Size of 'enum' = %ld bytes", sizeof(myEnum));
    printf("\n Size of 'macro' = %ld bytes", sizeof(MYMACRO));
    printf("\n\n");

    return 0;
}