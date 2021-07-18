// 24 Spheres with different materials in WebGL
// By : Darshan Vikam
// Date : 16 July 2021

// Global variables
var canvas = null;
var gl = null;
var canvas_original_width = 0;
var canvas_original_height = 0;
var bFullscreen = false;
var requestAnimationFrame = window.requestAnimationFrame ||	// For Google Chrome
	window.webkitRequestAnimationFrame ||			// For Apple's Safari
	window.mozRequestAnimationFrame ||			// For Mozilla FireFox
	window.oRequestAnimationFrame ||			// For Opera
	window.msRequestAnimationFrame;				// For MicroSoft's Edge

const WebGLMacros = {
	DV_ATTRIB_VERTEX	:0,
	DV_ATTRIB_COLOR		:1,
	DV_ATTRIB_NORMAL	:2,
	DV_ATTRIB_TEXTURE	:3
};

var VSObj, FSObj, SPObj;
var VAObj;
var VBObj = new Array(2);

var MMatrixUniform, VMatrixUniform, PMatrixUniform;
var LAmbUniform, LDiffUniform, LSpecUniform, LPosUniform;
var KAmbUniform, KDiffUniform, KSpecUniform, KShineUniform;
var lEnableUniform;
var sphereVertices, sphereNormals;

var angle = 0.0;
var XRotationEnabled, YRotationEnabled, ZRotationEnabled;
var lightEnabled = false;

var perspProjMatrix;

// Main function - entry point function 
function main() {
	// Get Canvas from DOM
	canvas = document.getElementById("DV");
	if(!canvas)
		console.log("Canvas not found \n");
	else
		console.log("Canvas obtained successfully.. \n");

	// Get canvas width and height
	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;

	// Setting Event listeners (for event handling)
	window.addEventListener("keydown", keyDown, false);	// window is an in-built variable
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);

	initialize();

	resize();	// warm up call to resize()
	display();	// warm up call to display() - explicitly
}

// Function to toggle fullscreen - multi browser complient
function toggleFullscreen() {
	// Variable declaration
	var fullscreen_element = document.fullscreenElement ||			// For Google Chrome, Opera
				 document.webkitFullscreenElement ||		// For Apple's - Safari
				 document.mozFullScreenElement ||		// For Mozilla firefox
				 document.msFullscreenElement ||		// For MicroSoft's - Edge
				 null;						// For any browser other that defined above OR if fullscreen is not set

	// Code
	// Need to specify fullscreen settings for other browsers
	if(fullscreen_element == null) { 		// If fullscreen not set
		// Check if function pointer is not null and then call that function
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.msFullscreen)
			canvas.msFullscreen();
		else
			alert("Fullscreen unavailable for this browser...");
		bFullscreen = true;
	}
	else {			// If fullscreen is set
		// Check if function pointer is not null and then call that function
		if(document.exitFullscreen)
			document.exitFullscreen();
		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();
		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		else if(document.msExitFullscreen)
			document.msExitFullscreen();
		bFullscreen = false;
	}
}

// Event listener / handler - for Keyboard event
function keyDown(event) {
	// message box in web
	switch(event.keyCode) {
		case 27 :	// Escape
			uninitialize();
			break;
		case 70 :	// f or F
			toggleFullscreen();
			break;
		case 76 :	// L or l (Lighting)
			lightEnabled = !lightEnabled;
			break;
		case 88 :	// x or X (X axis rotation)
			XRotationEnabled = true;
			YRotationEnabled = false;
			ZRotationEnabled = false;
			break;
		case 89 :	// y or Y (Y axis rotation)
			XRotationEnabled = false;
			YRotationEnabled = true;
			ZRotationEnabled = false;
			break;
		case 90 :	// z or Z (Z axis rotation)
			XRotationEnabled = false;
			YRotationEnabled = false;
			ZRotationEnabled = true;
			break;
		default :
			break;
	}
}

// Event handling - for mouse button events
function mouseDown() {
	// message box in web
}

// Function to initialize WebGL
function initialize() {
	// Code
	// Get drawing context from the canvas
	var webGLContext = canvas.getContext("webgl2");
	if(!webGLContext)
		console.log("WebGL2.0(latest version) context not found \n");
	else
		console.log("WebGL2.0(latest version) context obtained successfully.. \n");
	gl = webGLContext;

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	// Vertex Shader
	const VSSrcCode = 				// Source code of shader
		"#version 300 es \n" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" +
		"uniform vec4 u_LPos;" +
		"uniform bool u_lEnabled;" +
		"out vec3 tNorm, lSrc, viewVec;" +
		"void main(void) {" +
			"if(u_lEnabled) {" +
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" +
				"tNorm = mat3(u_VMatrix * u_MMatrix) * vNormal;" +
				"lSrc = vec3(u_LPos - eyeCoords);" +
				"viewVec = -eyeCoords.xyz;" +
			"}" +
			// Positioning
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" +
		"}";
	VSObj = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(VSObj, VSSrcCode);
	gl.compileShader(VSObj);
	ShaderErrorCheck(VSObj, "COMPILE");

	// Fragment Shader
	const FSSrcCode = 			// Source code of shader
		"#version 300 es \n" +
		"precision highp float;" + 
		"in vec3 tNorm, lSrc, viewVec;" +
		"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" +
		"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" +
		"uniform float u_KShine;" +
		"uniform bool u_lEnabled;" +
		"out vec4 FragColor;" +
		"void main(void) {" +
			"vec3 light = vec3(0.0);" +
			"if(u_lEnabled) {" +
				"vec3 transformedNormal = normalize(tNorm);" +
				"vec3 lightSource = normalize(lSrc);" +
				"vec3 viewVector = normalize(viewVec);" +
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" +
				"vec3 ambient = u_LAmb * u_KAmb;" +
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0);" +
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0), u_KShine);" +
				"light = ambient + diffuse + specular;" +
			"}" +
			"FragColor = vec4(light, 1.0);" +
		"}";
	FSObj = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(FSObj, FSSrcCode);
	gl.compileShader(FSObj);
	ShaderErrorCheck(FSObj, "COMPILE");

	// Shader Program
	SPObj = gl.createProgram();
	gl.attachShader(SPObj, VSObj);
	gl.attachShader(SPObj, FSObj);
	gl.bindAttribLocation(SPObj, WebGLMacros.DV_ATTRIB_VERTEX, "vPosition");
	gl.bindAttribLocation(SPObj, WebGLMacros.DV_ATTRIB_NORMAL, "vNormal");
	gl.linkProgram(SPObj);
	ShaderErrorCheck(SPObj, "LINK");

	// Get uniform locations
	MMatrixUniform = gl.getUniformLocation(SPObj, "u_MMatrix");
	VMatrixUniform = gl.getUniformLocation(SPObj, "u_VMatrix");
	PMatrixUniform = gl.getUniformLocation(SPObj, "u_PMatrix");
	LAmbUniform = gl.getUniformLocation(SPObj, "u_LAmb");
	LDiffUniform = gl.getUniformLocation(SPObj, "u_LDiff");
	LSpecUniform = gl.getUniformLocation(SPObj, "u_LSpec");
	LPosUniform = gl.getUniformLocation(SPObj, "u_LPos");
	KAmbUniform = gl.getUniformLocation(SPObj, "u_KAmb");
	KDiffUniform = gl.getUniformLocation(SPObj, "u_KDiff");
	KSpecUniform = gl.getUniformLocation(SPObj, "u_KSpec");
	KShineUniform = gl.getUniformLocation(SPObj, "u_KShine");
	LEnableUniform = gl.getUniformLocation(SPObj, "u_lEnabled");

	// Initializing vertices, color, shader attibs, VAO, VBO
	var lats = 30;
	var longs = 30;
	sphereVertices = getSphereVertices(1.0, lats, longs);
	sphereNormals = getSphereNormals(lats, longs);

	VAObj = gl.createVertexArray();
	gl.bindVertexArray(VAObj);
		VBObj[0] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj[0]);
		gl.bufferData(gl.ARRAY_BUFFER, sphereVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		VBObj[1] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj[1]);
		gl.bufferData(gl.ARRAY_BUFFER, sphereNormals, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_NORMAL, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_NORMAL);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

	// Clear canvas color to black
	gl.clearColor(0.1, 0.1, 0.1, 1.0);

	perspProjMatrix = mat4.create();
}

// Function to check errors in shaders after compiling/linking
function ShaderErrorCheck(shaderObject, status) {
	// Code
	if(status == "COMPILE") {
		if(gl.getShaderParameter(shaderObject, gl.COMPILE_STATUS) == false) {
			var error = gl.getShaderInfoLog(shaderObject);
			if(error.length > 0) {
				alert(error);
				uninitialize();
			}
		}
	}
	else if(status == "LINK") {
	if(gl.getProgramParameter(shaderObject, gl.LINK_STATUS) == false) {
			var error = gl.getProgramInfoLog(shaderObject);
			if(error.length > 0) {
				alert(error);
				uninitialize();
			}
		}
	}
	else
		alert("Invalid 2nd parameter of function ShaderErrorCheck().");
}

// Function to set 
function resize() {
	// Code
	if(bFullscreen) {
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else {
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}
//	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspProjMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

// Function draw
function display() {
	// Code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(SPObj);

	for(var i = 0; i < 4; i++) {
		for(var j = 0; j < 6; j++) {
			var MMatrix = mat4.create();
			var VMatrix = mat4.create();
			var PMatrix = perspProjMatrix;
			var TranslationMatrix = mat4.create();

			gl.viewport((canvas.width / 4) * i, (canvas.height / 6) * (5 - j), canvas.width / 4, canvas.height / 6);

			if(lightEnabled) {
				var lightAmbient = Array(0.0, 0.0, 0.0);
				var lightDiffuse = Array(1.0, 1.0, 1.0);
				var lightSpecular = Array(1.0, 1.0, 1.0);
				var lightPosition = Array(4);
				var radius = 10.0;
				if(XRotationEnabled) {
					lightPosition[0] = 0.0;
					lightPosition[1] = radius * Math.cos(toRadians(angle));
					lightPosition[2] = radius * Math.sin(toRadians(angle));
					lightPosition[3] = 1.0;
				}
				else if(YRotationEnabled) {
					lightPosition[0] = radius * Math.sin(toRadians(angle));
					lightPosition[1] = 0.0;
					lightPosition[2] = radius * Math.cos(toRadians(angle));
					lightPosition[3] = 1.0;
				}
				else if(ZRotationEnabled) {
					lightPosition[0] = radius * Math.cos(toRadians(angle));
					lightPosition[1] = radius * Math.sin(toRadians(angle));
					lightPosition[2] = 0.0;
					lightPosition[3] = 1.0;
				}
				else {
					lightPosition[0] = radius;
					lightPosition[1] = radius;
					lightPosition[2] = radius;
					lightPosition[3] = 1.0;
				}
				var materialAmbient = getMaterialAmbient(i, j);
				var materialDiffuse = getMaterialDiffuse(i, j);
				var materialSpecular = getMaterialSpecular(i, j);
				var materialShininess = getMaterialShininess(i, j);
				
				gl.uniform1i(LEnableUniform, true);
				gl.uniform3fv(LAmbUniform, lightAmbient);
				gl.uniform3fv(LDiffUniform, lightDiffuse);
				gl.uniform3fv(LSpecUniform, lightSpecular);
				gl.uniform4fv(LPosUniform, lightPosition);
				gl.uniform3fv(KAmbUniform, materialAmbient);
				gl.uniform3fv(KDiffUniform, materialDiffuse);
				gl.uniform3fv(KSpecUniform, materialSpecular);
				gl.uniform1f(KShineUniform, materialShininess);
			}
			else
				gl.uniform1i(LEnableUniform, false);

			mat4.translate(TranslationMatrix, TranslationMatrix, [0.0, 0.0, -3.0]);
			mat4.multiply(MMatrix, MMatrix, TranslationMatrix);
			gl.uniformMatrix4fv(MMatrixUniform, false, MMatrix);
			gl.uniformMatrix4fv(VMatrixUniform, false, VMatrix);
			gl.uniformMatrix4fv(PMatrixUniform, false, PMatrix);
			gl.bindVertexArray(VAObj);
				gl.drawArrays(gl.TRIANGLES, 0, sphereVertices.length);
			gl.bindVertexArray(null);
		}
	}

	gl.useProgram(null);

	if(XRotationEnabled || YRotationEnabled || ZRotationEnabled)
		angle += 1.0;
	if(angle >= 360.0)
		angle = 0.0;

	// Similar to glFlush / swapBuffers / glXSwapBuffers / Game loop
	requestAnimationFrame(display, canvas);
}

function toRadians(degree) {
	return (degree * Math.PI / 180.0);
}

function getMaterialAmbient(column, row) {
	var materialAmbient = Array(24);
	materialAmbient[0] = Array(0.0215, 0.1745, 0.0215);	// 1R 1C - Emerald
	materialAmbient[1] = Array(0.135, 0.2225, 0.1575);	// 2R 1C - Jade
	materialAmbient[2] = Array(0.05375, 0.05, 0.06625);	// 3R 1C - Obsidian
	materialAmbient[3] = Array(0.25, 0.20725, 0.20725);	// 4R 1C - Pearl
	materialAmbient[4] = Array(0.1745, 0.01175, 0.01175);	// 5R 1C - Ruby
	materialAmbient[5] = Array(0.1, 0.18725, 0.1745);	// 6R 1C - Turquoise
	materialAmbient[6] = Array(0.329412, 0.223529, 0.027451);// 1R 2C - Brass
	materialAmbient[7] = Array(0.2125, 0.1275, 0.054);	// 2R 2C - Bronze
	materialAmbient[8] = Array(0.25, 0.25, 0.25);		// 3R 2C - Chrome
	materialAmbient[9] = Array(0.19125, 0.0735, 0.0225);	// 4R 2C - Copper
	materialAmbient[10] = Array(0.24725, 0.1995, 0.0745);	// 5R 2C - Gold
	materialAmbient[11] = Array(0.19225, 0.19225, 0.19225);	// 6R 2C - Silver
	materialAmbient[12] = Array(0.0, 0.0, 0.0);		// 1R 3C - Black plastic
	materialAmbient[13] = Array(0.0, 0.1, 0.06);		// 2R 3C - Cyan plastic
	materialAmbient[14] = Array(0.0, 0.0, 0.0);		// 3R 3C - Green plastic
	materialAmbient[15] = Array(0.0, 0.0, 0.0);		// 4R 3C - Red plastic
	materialAmbient[16] = Array(0.0, 0.0, 0.0);		// 5R 3C - White plastic
	materialAmbient[17] = Array(0.0, 0.0, 0.0);		// 6R 3C - Yellow plastic
	materialAmbient[18] = Array(0.02, 0.02, 0.02);		// 1R 4C - Black rubber
	materialAmbient[19] = Array(0.0, 0.05, 0.05);		// 2R 4C - Cyan rubber
	materialAmbient[20] = Array(0.0, 0.05, 0.0);		// 3R 4C - Green rubber
	materialAmbient[21] = Array(0.05, 0.0, 0.0);		// 4R 4C - Red rubber
	materialAmbient[22] = Array(0.05, 0.05, 0.05);		// 5R 4C - White rubber
	materialAmbient[23] = Array(0.05, 0.05, 0.04);		// 6R 4C - Yellow rubber

	return materialAmbient[(column * 6) + row];
}

function getMaterialDiffuse(column, row) {
	var materialDiffuse = Array(24);
	materialDiffuse[0] = Array(0.07568, 0.61424, 0.07568);	// 1R 1C - Emerald
	materialDiffuse[1] = Array(0.54, 0.89, 0.63);		// 2R 1C - Jade
	materialDiffuse[2] = Array(0.18275, 0.17, 0.22525);	// 3R 1C - Obsidian
	materialDiffuse[3] = Array(1.0, 0.829, 0.829);		// 4R 1C - Pearl
	materialDiffuse[4] = Array(0.61424, 0.04136, 0.04163);	// 5R 1C - Ruby
	materialDiffuse[5] = Array(0.396, 0.74151, 0.69102);	// 6R 1C - Turquoise
	materialDiffuse[6] = Array(0.780392, 0.568627, 0.113725);// 1R 2C - Brass
	materialDiffuse[7] = Array(0.714, 0.4284, 0.18144);	// 2R 2C - Bronze
	materialDiffuse[8] = Array(0.4, 0.4, 0.4);		// 3R 2C - Chrome
	materialDiffuse[9] = Array(0.7038, 0.27048, 0.0828);	// 4R 2C - Copper
	materialDiffuse[10] = Array(0.75164, 0.60648, 0.22648);	// 5R 2C - Gold
	materialDiffuse[11] = Array(0.50754, 0.50754, 0.50754);	// 6R 2C - Silver
	materialDiffuse[12] = Array(0.01, 0.01, 0.01);		// 1R 3C - Black plastic
	materialDiffuse[13] = Array(0.0, 0.50980392, 0.50980392);	// 2R 3C - Cyan plastic
	materialDiffuse[14] = Array(0.1, 0.35, 0.1);		// 3R 3C - Green plastic
	materialDiffuse[15] = Array(0.5, 0.0, 0.0);		// 4R 3C - Red plastic
	materialDiffuse[16] = Array(0.55, 0.55, 0.55);		// 5R 3C - White plastic
	materialDiffuse[17] = Array(0.5, 0.5, 0.0);		// 6R 3C - Yellow plastic
	materialDiffuse[18] = Array(0.01, 0.01, 0.01);		// 1R 4C - Black rubber
	materialDiffuse[19] = Array(0.4, 0.5, 0.5);		// 2R 4C - Cyan rubber
	materialDiffuse[20] = Array(0.4, 0.5, 0.4);		// 3R 4C - Green rubber
	materialDiffuse[21] = Array(0.5, 0.4, 0.4);		// 4R 4C - Red rubber
	materialDiffuse[22] = Array(0.5, 0.5, 0.5);		// 5R 4C - White rubber
	materialDiffuse[23] = Array(0.5, 0.5, 0.4);		// 6R 4C - Yellow rubber

	return materialDiffuse[(column * 6) + row];
}

function getMaterialSpecular(column, row) {
	var materialSpecular = Array(24);
	materialSpecular[0] = Array(0.633, 0.727811, 0.33);		// 1R 1C - Emerald
	materialSpecular[1] = Array(0.316228, 0.316228, 0.316228);	// 2R 1C - Jade
	materialSpecular[2] = Array(0.332741, 0.328634, 0.346435);	// 3R 1C - Obsidian
	materialSpecular[3] = Array(0.296648, 0.296648, 0.296648);	// 4R 1C - Pearl
	materialSpecular[4] = Array(0.727811, 0.626959, 0.626959);	// 5R 1C - Ruby
	materialSpecular[5] = Array(0.297254, 0.308290, 0.306678);	// 6R 1C - Turquoise
	materialSpecular[6] = Array(0.992157, 0.941176, 0.807843);	// 1R 2C - Brass
	materialSpecular[7] = Array(0.393548, 0.271906, 0.166721);	// 2R 2C - Bronze
	materialSpecular[8] = Array(0.774597, 0.774597, 0.774597);	// 3R 2C - Chrome
	materialSpecular[9] = Array(0.256777, 0.137622, 0.086014);	// 4R 2C - Copper
	materialSpecular[10] = Array(0.628281, 0.555802, 0.366065);	// 5R 2C - Gold
	materialSpecular[11] = Array(0.508273, 0.508273, 0.508273);	// 6R 2C - Silver
	materialSpecular[12] = Array(0.5, 0.5, 0.5);			// 1R 3C - Black plastic
	materialSpecular[13] = Array(0.50196078, 0.50196078, 0.50196078);	// 2R 3C - Cyan plastic
	materialSpecular[14] = Array(0.45, 0.55, 0.45);			// 3R 3C - Green plastic
	materialSpecular[15] = Array(0.7, 0.6, 0.6);			// 4R 3C - Red plastic
	materialSpecular[16] = Array(0.7, 0.7, 0.7);			// 5R 3C - White plastic
	materialSpecular[17] = Array(0.6, 0.6, 0.5);			// 6R 3C - Yellow plastic
	materialSpecular[18] = Array(0.4, 0.4, 0.4);			// 1R 4C - Black rubber
	materialSpecular[19] = Array(0.04, 0.7, 0.7);			// 2R 4C - Cyan rubber
	materialSpecular[20] = Array(0.04, 0.7, 0.04);			// 3R 4C - Green rubber
	materialSpecular[21] = Array(0.7, 0.04, 0.04);			// 4R 4C - Red rubber
	materialSpecular[22] = Array(0.7, 0.7, 0.7);			// 5R 4C - White rubber
	materialSpecular[23] = Array(0.7, 0.7, 0.04);			// 6R 4C - Yellow rubber

	return materialSpecular[(column * 6) + row];
}

function getMaterialShininess(column, row) {
	var materialShininess = Array(
		0.6,		// 1R 1C - Emerald
		0.1,		// 2R 1C - Jade
		0.3,		// 3R 1C - Obsidian
		0.088,		// 4R 1C - Pearl
		0.6,		// 5R 1C - Ruby
		0.1,		// 6R 1C - Turquoise
		0.21794872,	// 1R 2C - Brass
		0.2,		// 2R 2C - Bronze
		0.6,		// 3R 2C - Chrome
		0.1,		// 4R 2C - Copper
		0.4,		// 5R 2C - Gold
		0.4,		// 6R 2C - Silver
		0.25,		// 1R 3C - Black plastic
		0.25,		// 2R 3C - Cyan plastic
		0.25,		// 3R 3C - Green plastic
		0.25,		// 4R 3C - Red plastic
		0.25,		// 5R 3C - White plastic
		0.25,		// 6R 3C - Yellow plastic
		0.078125,	// 1R 4C - Black rubber
		0.078125,	// 2R 4C - Cyan rubber
		0.078125,	// 3R 4C - Green rubber
		0.078125,	// 4R 4C - Red rubber
		0.078125,	// 5R 4C - White rubber
		0.078125	// 6R 4C - Yellow rubber
	)

	return (materialShininess[(column * 6) + row] * 128.0);
}

// Function uninitialize
function uninitialize() {
	// Code
	// Delete Vertex Array Object
	if(VAObj) {
		gl.deleteVertexArray(VAObj);
		VAObj = null;
	}

	// Delete Vertex Buffer Object
	if(VBObj) {
		gl.deleteBuffer(VBObj);
		VBObj = null;
	}

	// Delete shader programs and shaders
	if(SPObj) {
		if(FSObj) {
			gl.detachShader(SPObj, FSObj);
			gl.deleteShader(FSObj);
			FSObj = null;
		}
		if(VSObj) {
			gl.detachShader(SPObj, VSObj);
			gl.deleteShader(VSObj);
			VSObj = null;
		}
		gl.deleteProgram(SPObj);
		SPObj = null;
	}
	window.close();
}
