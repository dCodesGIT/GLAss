// OpenCL kernel
__kernel void matrixMultiply(__global int *A, __global int *B, __global int *C, int numARows, int numACols, int numBRows, int numBCols, int numCRows, int numCCols) {
	// Variable declaration
	int row = get_global_id(0);
	int col = get_global_id(1);
	
	// Code
	if((row < numARows) && (col < numBCols)) {
		int CValue = 0;
		for(int k = 0; k < numACols; k++)
			CValue += A[row * numACols + k] * B[k * numBCols + col];
		C[row * numCCols + col] = CValue;
	}
}
