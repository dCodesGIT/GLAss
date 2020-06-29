// To demonstrate pointers - typedef with struct pointers
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
struct MyData {
    int i;
    float f;
    double d;
 };

int main() {
    // Variable declaration
    int iSize, fSize, dSize;
    int structSize, structPointerSize;
    typedef struct MyData *MDP;
    MDP pData = NULL;

    // Code
    pData = (MDP)malloc(sizeof(struct MyData));
    if(pData == NULL) {
        printf("\n Memory allocation failed !!!");
        exit(0);
    }

    pData->i = 10;
    pData->f = 10.02f;
    pData->d = 2000.1002;
    printf("\n Data Members of 'struct MyData' are : ");
    printf("\n i = %d", pData->i);
    printf("\n f = %f", pData->f);
    printf("\n d = %lf", pData->d);

    iSize = sizeof(pData->i);
    fSize = sizeof(pData->f);
    dSize = sizeof(pData->d);
    printf("\n\n Sizes (in bytes) of members of 'struct MyData' are");
    printf("\n Size of 'i' = %d bytes", iSize);
    printf("\n Size of 'f' = %d bytes", fSize);
    printf("\n Size of 'd' = %d bytes", dSize);

    structSize = sizeof(struct MyData);
    structPointerSize = sizeof(MDP);
    printf("\n Size of 'struct MyData' = %d bytes", structSize);
    printf("\n Size of pointer of 'struct MyData' = %d bytes", structPointerSize);

    if(pData) {
        free(pData);
        pData = NULL;
    }
    
    printf("\n\n");
    return 0;
}