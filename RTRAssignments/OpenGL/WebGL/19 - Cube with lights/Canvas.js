// 3D Cube with lights in WebGL
// By : Darshan Vikam
// Date : 14 July 2021

// Global variables
var canvas = null;
var gl = null;
var canvas_original_width = 0;
var canvas_original_height = 0;
var bFullscreen = false;
var angle = 0.0;

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
var VAObj_cube;
var VBObj_cube = new Array(2);
var MMatrixUniform, VMatrixUniform, PMatrixUniform;
var animationEnabled = false;
var lightEnabled = false;
var LdUniform, LPosUniform;
var KdUniform;
var LEnabled;

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
	resize();
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
		case 76 :	// L or l (lighting)
			lightEnabled = !lightEnabled;
			break;
		case 65 :	// A or a (animation)
			animationEnabled = !animationEnabled;
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
	var VSSrcCode =
		"#version 300 es \n" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" +
		"uniform vec3 u_Ld, u_Kd;" +
		"uniform vec4 u_LPos;" +
		"uniform bool u_LEnabled;" +
		"out vec3 out_light;" +
		"void main(void) {" +
			"if(u_LEnabled == true) {" +
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" +
				"mat3 normalMatrix = mat3(transpose(inverse(u_VMatrix * u_MMatrix)));" +
				"vec3 transformedNormal = normalize(normalMatrix * vNormal);" +
				"vec3 lightSource = normalize(vec3(u_LPos - eyeCoords));" +
				"out_light = u_Ld * u_Kd * max(dot(lightSource, transformedNormal), 0.0);" +
			"}" +
			"else {" +
				"out_light = vec3(1.0);" +
			"}" +
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" +
		"}";
	VSObj = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(VSObj, VSSrcCode);
	gl.compileShader(VSObj);
	ShaderErrorCheck(VSObj, "COMPILE");

	// Fragment Shader
	var FSSrcCode =
		"#version 300 es \n" +
		"precision highp float;" +
		"in vec3 out_light;" +
		"out vec4 FragColor;" +
		"void main(void) {" +
			"FragColor = vec4(out_light, 1.0);" +
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
	LdUniform = gl.getUniformLocation(SPObj, "u_Ld");
	KdUniform = gl.getUniformLocation(SPObj, "u_Kd");
	LPosUniform = gl.getUniformLocation(SPObj, "u_LPos");
	LEnabled = gl.getUniformLocation(SPObj, "u_LEnabled");

	// Initializing vertices, color, shader attibs, VAO, VBO
	const cubeVertices = new Float32Array([
		// Front face - top left, bottom left, bottom right, top right
		-1.0, 1.0, 1.0,		-1.0, -1.0, 1.0,	1.0, -1.0, 1.0,		1.0, 1.0, 1.0,
		
		// Right face - top left, bottom left, bottom right, top right
		1.0, 1.0, 1.0,		1.0, -1.0, 1.0,		1.0, -1.0, -1.0,	1.0, 1.0, -1.0,
		
		// Bottom face - top left, bottom left, bottom right, top right
		1.0, -1.0, 1.0,		-1.0, -1.0, 1.0,	-1.0, -1.0, -1.0,	1.0, -1.0, -1.0,
		
		// Left face - top left, bottom left, bottom right, top right
		-1.0, 1.0, -1.0,	-1.0, -1.0, -1.0,	-1.0, -1.0, 1.0,	-1.0, 1.0, 1.0,
		
		// Back face - top left, bottom left, bottom right, top right
		1.0, 1.0, -1.0,		1.0, -1.0, -1.0,	-1.0, -1.0, -1.0,	-1.0, 1.0, -1.0,
		
		// Top face - top left, bottom left, bottom right, top right
		1.0, 1.0, 1.0,		1.0, 1.0, -1.0,		-1.0, 1.0, -1.0,	-1.0, 1.0, 1.0
	]);
	const cubeNormal = new Float32Array([
		// Front face
		0.0, 0.0, 1.0,		0.0, 0.0, 1.0,		0.0, 0.0, 1.0,		0.0, 0.0, 1.0,
		
		// Right face
		1.0, 0.0, 0.0,		1.0, 0.0, 0.0,		1.0, 0.0, 0.0,		1.0, 0.0, 0.0,
		
		// Bottom face
		0.0, -1.0, 0.0,		0.0, -1.0, 0.0,		0.0, -1.0, 0.0,		0.0, -1.0, 0.0,
		
		// Left face
		-1.0, 0.0, 0.0,		-1.0, 0.0, 0.0,		-1.0, 0.0, 0.0,		-1.0, 0.0, 0.0,
		
		// Back face
		0.0, 0.0, -1.0,		0.0, 0.0, -1.0,		0.0, 0.0, -1.0,		0.0, 0.0, -1.0,
		
		// Top face
		0.0, 1.0, 0.0,		0.0, 1.0, 0.0,		0.0, 1.0, 0.0,		0.0, 1.0, 0.0
	]);

	VAObj_cube = gl.createVertexArray();		// Pyramid
	gl.bindVertexArray(VAObj_cube);
		VBObj_cube[0] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_cube[0]);
		gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		VBObj_cube[1] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_cube[1]);
		gl.bufferData(gl.ARRAY_BUFFER, cubeNormal, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_NORMAL, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_NORMAL);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	gl.enable(gl.DEPTH_TEST);
	gl.clearDepth(1.0);
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
function display() {
	// Code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(SPObj);

	var MMatrix = mat4.create();
	var VMatrix = mat4.create();
	var PMatrix = perspProjMatrix;
	var TranslationMatrix, RotationMatrix;

	if(lightEnabled == true) {
		gl.uniform1i(LEnabled, true);
		gl.uniform3f(LdUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(KdUniform, 0.5, 0.5, 0.5);
		gl.uniform4f(LPosUniform, 0.0, 0.0, 2.0, 1.0);
	}
	else
		gl.uniform1i(LEnabled, false);

	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [0.0, 0.0, -5.0]);
	mat4.multiply(MMatrix, MMatrix, TranslationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, angle, [1.0, 0.0, 0.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, angle, [0.0, 1.0, 0.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, angle, [0.0, 0.0, 1.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);
	gl.uniformMatrix4fv(MMatrixUniform, false, MMatrix);
	gl.uniformMatrix4fv(VMatrixUniform, false, VMatrix);
	gl.uniformMatrix4fv(PMatrixUniform, false, PMatrix);
	gl.bindVertexArray(VAObj_cube);
		gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	gl.bindVertexArray(null);

	gl.useProgram(null);

	if(animationEnabled)
		angle += 0.01;
	if(angle >= 360.0)
		angle = 0.0;

	// Similar to glFlush / swapBuffers / glXSwapBuffers / Game loop
	requestAnimationFrame(display, canvas);
}

// Function uninitialize
function uninitialize() {
	// Code
	// Delete Vertex Array Object
	if(VAObj_cube) {
		gl.deleteVertexArray(VAObj_cube);
		VAObj_cube = null;
	}

	// Delete Vertex Buffer Object
	if(VBObj_cube) {
		gl.deleteBuffer(VBObj_cube);
		VBObj_cube = null;
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
