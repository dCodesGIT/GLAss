// Static India in WebGL
// By : Darshan Vikam
// Date : 04 July 2021

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
var VAObj_INDIA = new Array(4);
var VBObj_I = new Array(2);
var VBObj_N = new Array(2);
var VBObj_D = new Array(2);
var VBObj_A = new Array(2);
var MVPUniform;

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
		"in vec3 vColor;" +
		"uniform mat4 u_mvp_matrix;" +
		"out vec3 out_color;" +
		"void main(void) {" +
			"gl_Position = u_mvp_matrix * vPosition;" +
			"out_color = vColor;" +
		"}";
	VSObj = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(VSObj, VSSrcCode);
	gl.compileShader(VSObj);
	ShaderErrorCheck(VSObj, "COMPILE");

	// Fragment Shader
	var FSSrcCode =
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
	gl.bindAttribLocation(SPObj, WebGLMacros.DV_ATTRIB_COLOR, "vColor");
	gl.linkProgram(SPObj);
	ShaderErrorCheck(SPObj, "LINK");

	// Get uniform locations
	MVPUniform = gl.getUniformLocation(SPObj, "u_mvp_matrix");

	// Initializing vertices, color, shader attibs, VAO, VBO
	const alphabet_I_Vertices = new Float32Array([
		// Top quad of I
		-0.35, 0.5,	 0.35, 0.5,	 0.35, 0.4,	-0.35, 0.4,
		
		// Vertical quad (Upper half) of I
		-0.05, 0.4,	 0.05, 0.4,	 0.05, 0.0,	-0.05, 0.0,
		
		// Vertical quad (Lower half) of I
		 0.05, 0.0,	-0.05, 0.0,	-0.05, -0.4,	 0.05, -0.4,
		
		// Bottom quad of I
		-0.35, -0.5,	 0.35, -0.5,	 0.35, -0.4,	-0.35, -0.4
	]);
	const alphabet_I_Color = new Float32Array([
		// Top quad of I - Saffron
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,
		
		// Vertical quad (Upper half) of I - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// Vertical quad (Lower half) of I - White, White, Green, Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// Bottom quad of I - Green
		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03
	]);
	const alphabet_N_Vertices = new Float32Array([
		// 1st Vertical quad (Upper half)
		-0.35, 0.5,	-0.25, 0.5,	-0.25, 0.0,	-0.35, 0.0,
		
		// 1st Vertical quad (Lower half)
		-0.35, 0.0,	-0.25, 0.0,	-0.25, -0.5,	-0.35, -0.5,
		
		// Slant quad (Upper half)
		-0.35, 0.5,	-0.25, 0.5,	0.05, 0.0,	-0.05, 0.0,
		
		// Slant quad (lower half)
		0.05, 0.0,	-0.05, 0.0,	0.25, -0.5,	0.35, -0.5,
		
		// 2nd Vertical quad (upper half)
		0.35, 0.5,	0.25, 0.5,	0.25, 0.0,	0.35, 0.0,
		
		// 2nd Vertical quad (lower half)
		0.35, 0.0,	0.25, 0.0,	0.25, -0.5,	0.35, -0.5
	]);
	const alphabet_N_Color = new Float32Array([
		// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// 1st Vertical quad (Lower half) - White, White, Green, Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// Slant quad (Upper half) - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// Slant quad (lower half) - White, White, Green, Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// 2nd Vertical quad (upper half) - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// 2nd Vertical quad (lower half) - White, White, Green Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03
	]);
	const alphabet_D_Vertices = new Float32Array([
		// 1st Vertical quad (Upper half)
		-0.25, 0.4,	-0.15, 0.4,	-0.15, 0.0,	-0.25, 0.0,
		
		// 1st Vertical quad (Lower half)
		-0.25, 0.0,	-0.15, 0.0,	-0.15, -0.4,	-0.25, -0.4,
		
		// Upper horizontal quad
		-0.35, 0.5,	0.35, 0.5,	0.35, 0.4,	-0.35, 0.4,
		
		// 2nd Vertical quad (Upper half)
		0.35, 0.4,	0.25, 0.4,	0.25, 0.0,	0.35, 0.0,
		
		// 2nd Vertical quad (Lower half)
		0.35, 0.0,	0.25, 0.0,	0.25, -0.4,	0.35, -0.4,
		
		// Lower horizontal quad
		-0.35, -0.5,	0.35, -0.5,	0.35, -0.4,	-0.35, -0.4
	]);
	const alphabet_D_Color = new Float32Array([
		// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// 1st Vertical quad (Lower half) - White, White, Green, Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// Upper horizontal quad - Saffron
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,
		
		// 2nd Vertical quad (Upper half) - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// 2nd Vertical quad (Lower half) - White, White, Green, Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// Lower horizontal quad - Green
		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03
	]);
	const alphabet_A_Vertices = new Float32Array([
		// Central Horizontal
		0.25, 0.05,	0.25, -0.05,	-0.25, -0.05,	-0.25, 0.05,
		
		// 1st Vertical quad (Upper half)
		-0.35, 0.4,	-0.25, 0.4,	-0.25, 0.0,	-0.35, 0.0,
		
		// 1st Vertical quad (Lower half)
		-0.35, 0.0,	-0.25, 0.0,	-0.25, -0.4,	-0.35, -0.4,
		
		// Upper Horizontal quad
		0.35, 0.5,	0.35, 0.4,	-0.35, 0.4,	-0.35, 0.5,
		
		// 2nd Vertical quad (Upper half)
		0.35, 0.4,	0.25, 0.4,	0.25, 0.0,	0.35, 0.0,
		
		// 2nd Vertical quad (Lower half)
		0.35, 0.0,	0.25, 0.0,	0.25, -0.4,	0.35, -0.4,
		
		// 1st Vertical quad (Lowest part square)
		-0.25, -0.4,	-0.35, -0.4,	-0.35, -0.5,	-0.25, -0.5,
		
		// 2nd Vertical quad (Lowest part square)
		0.25, -0.4,	0.35, -0.4,	0.35, -0.5,	0.25, -0.5
	]);
	const alphabet_A_Color = new Float32Array([
		// Central Horizontal - White
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// 1st Vertical quad (Lower half) - White, White, Green, Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// Upper Horizontal quad - Saffron
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,
		
		// 2st Vertical quad (Upper half) - Saffron, Saffron, White, White
		1.0, 0.6, 0.2,		1.0, 0.6, 0.2,		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,
		
		// 2st Vertical quad (Lower half) - White, White, Green, Green
		1.0, 1.0, 1.0,		1.0, 1.0, 1.0,		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// 1st Vertical quad (Lowest part square) - Green
		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03,
		
		// 2st Vertical quad (Lowest part square) - Green
		0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03,	0.07, 0.53, 0.03
	]);

	VAObj_INDIA[0] = gl.createVertexArray();		// Alphabet I
	gl.bindVertexArray(VAObj_INDIA[0]);
		VBObj_I[0] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_I[0]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_I_Vertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
		
		VBObj_I[1] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_I[1]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_I_Color, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);
	
	VAObj_INDIA[1] = gl.createVertexArray();		// Alphabet N
	gl.bindVertexArray(VAObj_INDIA[1]);
		VBObj_N[0] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_N[0]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_N_Vertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
		
		VBObj_N[1] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_N[1]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_N_Color, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);
	
	VAObj_INDIA[2] = gl.createVertexArray();		// Alphabet D
	gl.bindVertexArray(VAObj_INDIA[2]);
		VBObj_D[0] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_D[0]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_D_Vertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
		
		VBObj_D[1] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_D[1]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_D_Color, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);
	
	VAObj_INDIA[3] = gl.createVertexArray();		// Alphabet A
	gl.bindVertexArray(VAObj_INDIA[3]);
		VBObj_A[0] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_A[0]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_A_Vertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
		
		VBObj_A[1] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_A[1]);
		gl.bufferData(gl.ARRAY_BUFFER, alphabet_A_Color, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

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
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(SPObj);

	var ModelViewMatrix = mat4.create();
	var ModelViewProjectionMatrix = mat4.create();
	var TranslationMatrix;

	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [0.0, 0.0, -3.0]);
	mat4.multiply(ModelViewMatrix, ModelViewMatrix, TranslationMatrix);

	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [-2.0, 0.0, 0.0]);
	mat4.multiply(ModelViewMatrix, ModelViewMatrix, TranslationMatrix);
	mat4.multiply(ModelViewProjectionMatrix, perspProjMatrix, ModelViewMatrix);
	gl.uniformMatrix4fv(MVPUniform, false, ModelViewProjectionMatrix);
	I();

	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [1.0, 0.0, 0.0]);
	mat4.multiply(ModelViewMatrix, ModelViewMatrix, TranslationMatrix);
	mat4.multiply(ModelViewProjectionMatrix, perspProjMatrix, ModelViewMatrix);
	gl.uniformMatrix4fv(MVPUniform, false, ModelViewProjectionMatrix);
	N();

	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [1.0, 0.0, 0.0]);
	mat4.multiply(ModelViewMatrix, ModelViewMatrix, TranslationMatrix);
	mat4.multiply(ModelViewProjectionMatrix, perspProjMatrix, ModelViewMatrix);
	gl.uniformMatrix4fv(MVPUniform, false, ModelViewProjectionMatrix);
	D();

	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [1.0, 0.0, 0.0]);
	mat4.multiply(ModelViewMatrix, ModelViewMatrix, TranslationMatrix);
	mat4.multiply(ModelViewProjectionMatrix, perspProjMatrix, ModelViewMatrix);
	gl.uniformMatrix4fv(MVPUniform, false, ModelViewProjectionMatrix);
	I();

	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [1.0, 0.0, 0.0]);
	mat4.multiply(ModelViewMatrix, ModelViewMatrix, TranslationMatrix);
	mat4.multiply(ModelViewProjectionMatrix, perspProjMatrix, ModelViewMatrix);
	gl.uniformMatrix4fv(MVPUniform, false, ModelViewProjectionMatrix);
	A();

	gl.useProgram(null);

	// Similar to glFlush / swapBuffers / glXSwapBuffers / Game loop
	requestAnimationFrame(display, canvas);
}

function I() {
	// Code
	gl.bindVertexArray(VAObj_INDIA[0]);
		gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.bindVertexArray(null);
}

function N() {
	// Code
	gl.bindVertexArray(VAObj_INDIA[1]);
		gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	gl.bindVertexArray(null);
}

function D() {
	// Code
	gl.bindVertexArray(VAObj_INDIA[2]);
		gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	gl.bindVertexArray(null);
}

function A() {
	// Code
	gl.bindVertexArray(VAObj_INDIA[3]);
		gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 24, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 28, 4);
	gl.bindVertexArray(null);
}

// Function uninitialize
function uninitialize() {
	// Code
	// Delete Vertex Array Object
	if(VAObj_INDIA) {
		gl.deleteVertexArray(VAObj_INDIA);
		VAObj_INDIA = null;
	}

	// Delete Vertex Buffer Object
	if(VBObj_I) {
		gl.deleteBuffer(VBObj_I);
		VBObj_I = null;
	}
	if(VBObj_N) {
		gl.deleteBuffer(VBObj_N);
		VBObj_N = null;
	}
	if(VBObj_D) {
		gl.deleteBuffer(VBObj_D);
		VBObj_D = null;
	}
	if(VBObj_A) {
		gl.deleteBuffer(VBObj_A);
		VBObj_A = null;
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
