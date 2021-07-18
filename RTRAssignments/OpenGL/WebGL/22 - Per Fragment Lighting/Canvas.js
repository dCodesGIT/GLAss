// Per Fragment Lighting in WebGL
// By : Darshan Vikam
// Date : 15 July 2021

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

var albedoEnabled = false;
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
		case 65 :	// a or A (albedo enabling)
			albedoEnabled = !albedoEnabled;
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
			"vec3 light;" +
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
			"else {" +
				"light = vec3(0.0);" +
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
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

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
	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspProjMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

// Function draw
var angle = 0.0;
function display() {
	// Code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(SPObj);

	var MMatrix = mat4.create();
	var VMatrix = mat4.create();
	var PMatrix = perspProjMatrix;
	var TranslationMatrix = mat4.create();

	var lightAmbient, lightDiffuse, lightSpecular, lightPosition;
	var materialAmbient, materialDiffuse, materialSpecular, materialShininess;
	if(albedoEnabled) {
		lightAmbient = Array(0.1, 0.1, 0.1);
		lightDiffuse = Array(1.0, 1.0, 1.0);
		lightSpecular = Array(1.0, 1.0, 1.0);
		lightPosition = Array(100.0, 100.0, 100.0, 1.0);
		materialAmbient = Array(0.0, 0.0, 0.0);
		materialDiffuse = Array(0.5, 0.2, 0.7);
		materialSpecular = Array(0.7, 0.7, 0.7);
		materialShininess = 128;
	}
	else {
		lightAmbient = Array(0.0, 0.0, 0.0);
		lightDiffuse = Array(1.0, 1.0, 1.0);
		lightSpecular = Array(1.0, 1.0, 1.0);
		lightPosition = Array(100.0, 100.0, 100.0, 1.0);
		materialAmbient = Array(0.0, 0.0, 0.0);
		materialDiffuse = Array(1.0, 1.0, 1.0);
		materialSpecular = Array(1.0, 1.0, 1.0);
		materialShininess = 50;
	}

	if(lightEnabled) {
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

	gl.useProgram(null);

	// Similar to glFlush / swapBuffers / glXSwapBuffers / Game loop
	requestAnimationFrame(display, canvas);
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
