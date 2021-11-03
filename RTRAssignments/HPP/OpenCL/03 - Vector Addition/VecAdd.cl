// OpenCL kernel
__kernel void vecAdd(__global float *in1, __global float *in2, __global float *out, int len) {
	// Variable declaration
	int i = get_global_id(0);

	// Code
	if(i < len)
		out[i] = in1[i] + in2[i];
}