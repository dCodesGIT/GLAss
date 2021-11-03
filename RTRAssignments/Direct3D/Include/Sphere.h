// Sphere for DirectX

// Header files
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

float *sphereVertices, *sphereColor, *sphereNormals, *sphereTexture;

int GenSphere(float radius, int lats, int longs) {
	// Constant declaration
	const float rad = M_PI / 180.0f;
	const float latitudes = 180.0f / (float)lats;
	const float longitudes = 360.0f / (float)longs;
	const float uDiff = 1.0 / (float)lats;
	const float vDiff = 1.0 / (float)(longs - 1);
	const int points = lats * longs * 6;
	int ptCntr = 0;
	
	// Code
	// Allocating memory for arrays
	sphereVertices = (float *)malloc(points * 3 * sizeof(float));
	sphereNormals = (float *)malloc(points * 3 * sizeof(float));
	sphereTexture = (float *)malloc(points * 2 * sizeof(float));
	// Setting of values
	for(float i = 0.0f, u = 0.0f; i < 360.0f; i += longitudes, u += uDiff) {
		float iNext = i + longitudes;
		float uNext = u + uDiff;
		for(float j = 0.0f, v = 0.0f; j < 180.0f; j += latitudes, v += vDiff) {
			float jNext = j + latitudes;
			float vNext = v + vDiff;
			float x[4], y[4], z[4];
			float Tex_U[4], Tex_V[4];
			
			// Calculating points
			x[0] = cos(i*rad) * sin(j*rad);
			y[0] = cos(j*rad);
			z[0] = sin(i*rad) * sin(j*rad);
			Tex_U[0] = u;
			Tex_V[0] = v;
			
			x[1] = cos(i*rad) * sin(jNext*rad);
			y[1] = cos(jNext*rad);
			z[1] = sin(i*rad) * sin(jNext*rad);
			Tex_U[1] = u;
			Tex_V[1] = vNext;
			
			x[2] = cos(iNext*rad) * sin(jNext*rad);
			y[2] = cos(jNext*rad);
			z[2] = sin(iNext*rad) * sin(jNext*rad);
			Tex_U[2] = uNext;
			Tex_V[2] = vNext;
			
			x[3] = cos(iNext*rad) * sin(j*rad);
			y[3] = cos(j*rad);
			z[3] = sin(iNext*rad) * sin(j*rad);
			Tex_U[3] = uNext;
			Tex_V[3] = v;
			
			// Triangle 1 (0, 1, 2)
			sphereVertices[(ptCntr*3)+0] = radius * x[0];
			sphereVertices[(ptCntr*3)+1] = radius * y[0];
			sphereVertices[(ptCntr*3)+2] = radius * z[0];
			sphereNormals[(ptCntr*3)+0] = x[0];
			sphereNormals[(ptCntr*3)+1] = y[0];
			sphereNormals[(ptCntr*3)+2] = z[0];
			sphereTexture[(ptCntr*2)+0] = Tex_U[0];
			sphereTexture[(ptCntr*2)+1] = Tex_V[0];
			ptCntr++;
			
			sphereVertices[(ptCntr*3)+0] = radius * x[1];
			sphereVertices[(ptCntr*3)+1] = radius * y[1];
			sphereVertices[(ptCntr*3)+2] = radius * z[1];
			sphereNormals[(ptCntr*3)+0] = x[1];
			sphereNormals[(ptCntr*3)+1] = y[1];
			sphereNormals[(ptCntr*3)+2] = z[1];
			sphereTexture[(ptCntr*2)+0] = Tex_U[1];
			sphereTexture[(ptCntr*2)+1] = Tex_V[1];
			ptCntr++;
			
			sphereVertices[(ptCntr*3)+0] = radius * x[2];
			sphereVertices[(ptCntr*3)+1] = radius * y[2];
			sphereVertices[(ptCntr*3)+2] = radius * z[2];
			sphereNormals[(ptCntr*3)+0] = x[2];
			sphereNormals[(ptCntr*3)+1] = y[2];
			sphereNormals[(ptCntr*3)+2] = z[2];
			sphereTexture[(ptCntr*2)+0] = Tex_U[2];
			sphereTexture[(ptCntr*2)+1] = Tex_V[2];
			ptCntr++;
			
			// Triangle 2 (2, 3, 0)
			sphereVertices[(ptCntr*3)+0] = radius * x[2];
			sphereVertices[(ptCntr*3)+1] = radius * y[2];
			sphereVertices[(ptCntr*3)+2] = radius * z[2];
			sphereNormals[(ptCntr*3)+0] = x[2];
			sphereNormals[(ptCntr*3)+1] = y[2];
			sphereNormals[(ptCntr*3)+2] = z[2];
			sphereTexture[(ptCntr*2)+0] = Tex_U[2];
			sphereTexture[(ptCntr*2)+1] = Tex_V[2];
			ptCntr++;
			
			sphereVertices[(ptCntr*3)+0] = radius * x[3];
			sphereVertices[(ptCntr*3)+1] = radius * y[3];
			sphereVertices[(ptCntr*3)+2] = radius * z[3];
			sphereNormals[(ptCntr*3)+0] = x[3];
			sphereNormals[(ptCntr*3)+1] = y[3];
			sphereNormals[(ptCntr*3)+2] = z[3];
			sphereTexture[(ptCntr*2)+0] = Tex_U[3];
			sphereTexture[(ptCntr*2)+1] = Tex_V[3];
			ptCntr++;
			
			sphereVertices[(ptCntr*3)+0] = radius * x[0];
			sphereVertices[(ptCntr*3)+1] = radius * y[0];
			sphereVertices[(ptCntr*3)+2] = radius * z[0];
			sphereNormals[(ptCntr*3)+0] = x[0];
			sphereNormals[(ptCntr*3)+1] = y[0];
			sphereNormals[(ptCntr*3)+2] = z[0];
			sphereTexture[(ptCntr*2)+0] = Tex_U[0];
			sphereTexture[(ptCntr*2)+1] = Tex_V[0];
			ptCntr++;
		}
	}
	return ptCntr;
}

float *setSphereColor(int size, float red, float green, float blue) {
	// Code
	sphereColor = (float *)malloc(size * 3 * sizeof(float));
	for(int i = 0; i < size; i++) {
		sphereColor[(i*3)+0] = red;
		sphereColor[(i*3)+1] = green;
		sphereColor[(i*3)+2] = blue;
	}
	return sphereColor;
}

float *getSphereVertices() {
	return sphereVertices;
}

float *getSphereNormals() {
	return sphereNormals;
}

float *getSphereTexCoords() {
	return sphereTexture;
}

void deleteSphere() {
	if(sphereVertices) {
		free(sphereVertices);
		sphereVertices = NULL;
	}
	if(sphereColor) {
		free(sphereColor);
		sphereColor = NULL;
	}
	if(sphereNormals) {
		free(sphereNormals);
		sphereNormals = NULL;
	}
	if(sphereTexture) {
		free(sphereTexture);
		sphereTexture = NULL;
	}
}
