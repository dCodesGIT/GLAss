//
//  sphere.h
//
//  Created by Darshan Vikam on 04/08/21.
//

// Importing required header files
#import <Foundation/Foundation.h>
#import <math.h>
#import <stdlib.h>

@interface MySphere : NSObject
	-(int)GenSphere : (float)radius stack : (int)lats slices : (int)longs;
	-(float *)setSphereColor : (int)size R : (float)red G : (float)green B : (float)blue;
	-(float *)getSphereVertices;
	-(float *)getSphereNormals;
	-(float *)getSphereTexCoords;
	-(void)deleteSphere;
@end

@implementation MySphere {
	@private
	float *sphereVertices, *sphereColor, *sphereNormals, *sphereTexture;
}
-(int)GenSphere : (float)radius stack : (int)lats slices : (int)longs {
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
			
			// Calculating points
			x[0] = cos(i*rad) * sin(j*rad);
			y[0] = cos(j*rad);
			z[0] = sin(i*rad) * sin(j*rad);

			x[1] = cos(iNext*rad) * sin(j*rad);
			y[1] = cos(j*rad);
			z[1] = sin(iNext*rad) * sin(j*rad);

			x[2] = cos(iNext*rad) * sin(jNext*rad);
			y[2] = cos(jNext*rad);
			z[2] = sin(iNext*rad) * sin(jNext*rad);

			x[3] = cos(i*rad) * sin(jNext*rad);
			y[3] = cos(jNext*rad);
			z[3] = sin(i*rad) * sin(jNext*rad);
			
			// Triangle 1 (0, 1, 2)
			sphereVertices[(ptCntr*3)+0] = radius * x[0];
			sphereVertices[(ptCntr*3)+1] = radius * y[0];
			sphereVertices[(ptCntr*3)+2] = radius * z[0];
			sphereNormals[(ptCntr*3)+0] = x[0];
			sphereNormals[(ptCntr*3)+1] = y[0];
			sphereNormals[(ptCntr*3)+2] = z[0];
			sphereTexture[(ptCntr*2)+0] = u;
			sphereTexture[(ptCntr*2)+1] = v;
			ptCntr++;

			sphereVertices[(ptCntr*3)+0] = radius * x[1];
			sphereVertices[(ptCntr*3)+1] = radius * y[1];
			sphereVertices[(ptCntr*3)+2] = radius * z[1];
			sphereNormals[(ptCntr*3)+0] = x[1];
			sphereNormals[(ptCntr*3)+1] = y[1];
			sphereNormals[(ptCntr*3)+2] = z[1];
			sphereTexture[(ptCntr*2)+0] = uNext;
			sphereTexture[(ptCntr*2)+1] = v;
			ptCntr++;

			sphereVertices[(ptCntr*3)+0] = radius * x[2];
			sphereVertices[(ptCntr*3)+1] = radius * y[2];
			sphereVertices[(ptCntr*3)+2] = radius * z[2];
			sphereNormals[(ptCntr*3)+0] = x[2];
			sphereNormals[(ptCntr*3)+1] = y[2];
			sphereNormals[(ptCntr*3)+2] = z[2];
			sphereTexture[(ptCntr*2)+0] = uNext;
			sphereTexture[(ptCntr*2)+1] = vNext;
			ptCntr++;

			// Triangle 2 (2, 3, 0)
			sphereVertices[(ptCntr*3)+0] = radius * x[2];
			sphereVertices[(ptCntr*3)+1] = radius * y[2];
			sphereVertices[(ptCntr*3)+2] = radius * z[2];
			sphereNormals[(ptCntr*3)+0] = x[2];
			sphereNormals[(ptCntr*3)+1] = y[2];
			sphereNormals[(ptCntr*3)+2] = z[2];
			sphereTexture[(ptCntr*2)+0] = uNext;
			sphereTexture[(ptCntr*2)+1] = vNext;
			ptCntr++;

			sphereVertices[(ptCntr*3)+0] = radius * x[3];
			sphereVertices[(ptCntr*3)+1] = radius * y[3];
			sphereVertices[(ptCntr*3)+2] = radius * z[3];
			sphereNormals[(ptCntr*3)+0] = x[3];
			sphereNormals[(ptCntr*3)+1] = y[3];
			sphereNormals[(ptCntr*3)+2] = z[3];
			sphereTexture[(ptCntr*2)+0] = u;
			sphereTexture[(ptCntr*2)+1] = vNext;
			ptCntr++;
			
			sphereVertices[(ptCntr*3)+0] = radius * x[0];
			sphereVertices[(ptCntr*3)+1] = radius * y[0];
			sphereVertices[(ptCntr*3)+2] = radius * z[0];
			sphereNormals[(ptCntr*3)+0] = x[0];
			sphereNormals[(ptCntr*3)+1] = y[0];
			sphereNormals[(ptCntr*3)+2] = z[0];
			sphereTexture[(ptCntr*2)+0] = u;
			sphereTexture[(ptCntr*2)+1] = v;
			ptCntr++;
		}
	}
	return ptCntr;
}
-(float *)setSphereColor : (int)size R : (float)red G : (float)green B : (float)blue {
	// Code
	sphereColor = (float *)malloc(size * 3 * sizeof(float));
	for(int i = 0; i < size; i++) {
		sphereColor[(i*3)+0] = red;
		sphereColor[(i*3)+1] = green;
		sphereColor[(i*3)+2] = blue;
	}
	return sphereColor;
}
-(float *)getSphereVertices {
	return sphereVertices;
}
-(float *)getSphereNormals {
	return sphereNormals;
}
-(float *)getSphereTexCoords {
	return sphereTexture;
}
-(void)deleteSphere {
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
@end
