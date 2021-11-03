// Header file for OpenCL
// By : Darshan Vikam

// Function to load OpenCL Kernel program as an array of strings.
char *loadOCLProgram(const char *filename, const char *preamble, size_t *finalLength) {
	// Variable declaration
	FILE *fp = NULL;
	size_t srcLen;

	// Code
	fp = fopen(filename, "rb");
	if(fp == NULL)
		return NULL;

	
	size_t preambleLen = (size_t)strlen(preamble);
	fseek(fp, 0, SEEK_END);
	srcLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *srcString = (char *)malloc(srcLen + preambleLen + 1);
	memcpy(srcString, preamble, preambleLen);
	if(fread((srcString) + preambleLen, srcLen, 1, fp) != 1) {
		fclose(fp);
		free(srcString);
		return NULL;
	}

	fclose(fp);
	if(finalLength != 0)
		*finalLength = srcLen + preambleLen;
	srcString[srcLen + preambleLen] = '\0';

	return srcString;
}
