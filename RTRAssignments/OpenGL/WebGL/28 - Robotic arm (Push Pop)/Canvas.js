// Robotic arm in WebGL
// By : Darshan Vikam
// Date : 05 July 2021

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
var VBObj;

var MMatrixUniform, VMatrixUniform, PMatrixUniform;
var ColorUniform;
var sphereVertices;

var elbow = 0, shoulder = 0;
const radians = Math.PI / 180.0;

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
		case 69 :	// E or e
			elbow += 6;
			if(elbow >= 360)
				elbow = 0;
			break;
		case 83 :	// S or s
			shoulder += 3;
			if(shoulder >= 360)
				shoulder  = 0;
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
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" +
		"uniform vec3 u_Color;" +
		"out vec3 out_color;" +
		"void main(void) {" +
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" +
			"out_color = u_Color;" +
		"}";
	VSObj = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(VSObj, VSSrcCode);
	gl.compileShader(VSObj);
	ShaderErrorCheck(VSObj, "COMPILE");

	// Fragment Shader
	const FSSrcCode = 			// Source code of shader
		"#version 300 es \n" +
		"precision highp float;" + 
		"in vec3 out_color;" +
		"out vec4 FragColor;" +
		"void main(void) {" +
			"FragColor = vec4(out_color, 1.0);" +
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
	gl.linkProgram(SPObj);
	ShaderErrorCheck(SPObj, "LINK");

	// Get uniform locations
	MMatrixUniform = gl.getUniformLocation(SPObj, "u_MMatrix");
	VMatrixUniform = gl.getUniformLocation(SPObj, "u_VMatrix");
	PMatrixUniform = gl.getUniformLocation(SPObj, "u_PMatrix");
	ColorUniform = gl.getUniformLocation(SPObj, "u_Color");

	// Initializing vertices, color, shader attibs, VAO, VBO
	var lats = 30;
	var longs = 30;
	sphereVertices = getSphereVertices(0.5, lats, longs);

	VAObj = gl.createVertexArray();
	gl.bindVertexArray(VAObj);
		VBObj = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj);
		gl.bufferData(gl.ARRAY_BUFFER, sphereVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
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
function display() {
	// Code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	var MMatrix = mat4.create();
	var VMatrix = mat4.create();
	var PMatrix = perspProjMatrix;
	var TranslationMatrix = mat4.create();
	var RotationMatrix = mat4.create();
	var ScalingMatrix = mat4.create();

	gl.useProgram(SPObj);
	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [0.0, 0.0, -10.0]);
	mat4.multiply(MMatrix, MMatrix, TranslationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, shoulder * radians, [0.0, 0.0, 1.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);
	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [1.0, 0.0, 0.0]);
	mat4.multiply(MMatrix, MMatrix, TranslationMatrix);
	pushMatrix(mat4.clone(MMatrix));
		ScalingMatrix = mat4.create();
		mat4.scale(ScalingMatrix, ScalingMatrix, [2.0, 0.65, 1.0]);
		mat4.multiply(MMatrix, MMatrix, ScalingMatrix);
		gl.uniformMatrix4fv(MMatrixUniform, false, MMatrix);
		gl.uniformMatrix4fv(VMatrixUniform, false, VMatrix);
		gl.uniformMatrix4fv(PMatrixUniform, false, PMatrix);
		gl.uniform3f(ColorUniform, 0.5, 0.35, 0.05);
		gl.bindVertexArray(VAObj);
			gl.drawArrays(gl.TRIANGLES, 0, sphereVertices.length);
		gl.bindVertexArray(null);
	MMatrix = popMatrix();
	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [1.0, 0.0, 0.0]);
	mat4.multiply(MMatrix, MMatrix, TranslationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, elbow * radians, [0.0, 0.0, 1.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);
	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [1.0, 0.0, 0.0]);
	mat4.multiply(MMatrix, MMatrix, TranslationMatrix);
	pushMatrix(mat4.clone(MMatrix));
		ScalingMatrix = mat4.create();
		mat4.scale(ScalingMatrix, ScalingMatrix, [2.0, 0.5, 1.0]);
		mat4.multiply(MMatrix, MMatrix, ScalingMatrix);
		gl.uniformMatrix4fv(MMatrixUniform, false, MMatrix);
		gl.uniformMatrix4fv(VMatrixUniform, false, VMatrix);
		gl.uniformMatrix4fv(PMatrixUniform, false, PMatrix);
		gl.uniform3f(ColorUniform, 0.5, 0.35, 0.05);
		gl.bindVertexArray(VAObj);
			gl.drawArrays(gl.TRIANGLES, 0, sphereVertices.length);
		gl.bindVertexArray(null);
	MMatrix = popMatrix();
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
