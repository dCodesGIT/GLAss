// Tweaked smiley in WebGL
// By : Darshan Vikam
// Date : 09 July 2021

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
var VAObj_quad;
var VBObj_quad = new Array(2);

var MVPUniform;
var TextureSamplerUniform;

var smiley_texture;
var keyPressed = 0;

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
		case 48 :	// 0
			keyPressed = 0;
			break;
		case 49 :	// 1
			keyPressed = 1;
			break;
		case 50 :	// 2
			keyPressed = 2;
			break;
		case 51 :	// 3
			keyPressed = 3;
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
		"in vec2 vTexCoord;" +
		"uniform mat4 u_mvp_matrix;" +
		"out vec2 out_texCoord;" +
		"void main(void) {" +
			"gl_Position = u_mvp_matrix * vPosition;" +
			"out_texCoord = vTexCoord;" +
		"}";
	VSObj = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(VSObj, VSSrcCode);
	gl.compileShader(VSObj);
	ShaderErrorCheck(VSObj, "COMPILE");

	// Fragment Shader
	var FSSrcCode =
		"#version 300 es \n" +
		"precision highp float;" +
		"in vec2 out_texCoord;" +
		"uniform sampler2D u_texture_sampler; " +
		"out vec4 FragColor;" +
		"void main(void) {" +
			"FragColor = texture(u_texture_sampler, out_texCoord);" +
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
	gl.bindAttribLocation(SPObj, WebGLMacros.DV_ATTRIB_TEXTURE, "vTexCoord");
	gl.linkProgram(SPObj);
	ShaderErrorCheck(SPObj, "LINK");

	// Get uniform locations
	MVPUniform = gl.getUniformLocation(SPObj, "u_mvp_matrix");
	TextureSamplerUniform = gl.getUniformLocation(SPObj, "u_texture_sampler");

	// Initializing vertices, color, shader attibs, VAO, VBO
	const quadVertices = new Float32Array([
		-1.0, 1.0, 0.0,		// top left
		-1.0, -1.0, 0.0,	// bottom left
		1.0, -1.0, 0.0,		// bottom right
		1.0, 1.0, 0.0		// top right
	]);
	const quadTexCoords = new Float32Array([
		0.0, 1.0,	// top left
		0.0, 0.0,	// bottom left
		1.0, 0.0,	// bottom right
		1.0, 1.0	// top right
	]);

	VAObj_quad = gl.createVertexArray();		// Quad
	gl.bindVertexArray(VAObj_quad);
		VBObj_quad[0] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_quad[0]);
		gl.bufferData(gl.ARRAY_BUFFER, quadVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		VBObj_quad[1] = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_quad[1]);
		gl.bufferData(gl.ARRAY_BUFFER, quadTexCoords.length, gl.DYNAMIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_TEXTURE, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_TEXTURE);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	gl.enable(gl.DEPTH_TEST);
	gl.clearDepth(1.0);
	gl.depthFunc(gl.LEQUAL);

	smiley_texture = loadTexture(gl, "smiley.png");

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

// function to load texture
function loadTexture(gl, texture_src) {
	// Code
	var texture = gl.createTexture();
	texture.image = new Image();
	texture.image.src = texture_src;
	texture.image.onload = function() {
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 0);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, texture.image);
		gl.generateMipmap(gl.TEXTURE_2D);
	}
	return texture;
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

	var ModelViewMatrix = mat4.create();
	var ModelViewProjectionMatrix = mat4.create();
	var TranslationMatrix;

	var quadTexCoords;
	if(keyPressed == 0) {
		quadTexCoords = new Float32Array([
			0.0, 1.0,	// top left
			0.0, 0.0,	// bottom left
			1.0, 0.0,	// bottom right
			1.0, 1.0	// top right
		]);
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_quad[1]);
		gl.bufferData(gl.ARRAY_BUFFER, quadTexCoords, gl.DYNAMIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_TEXTURE, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_TEXTURE);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	else if(keyPressed == 1) {
		quadTexCoords = new Float32Array([
			0.0, 0.5,	// top left
			0.0, 0.0,	// bottom left
			0.5, 0.0,	// bottom right
			0.5, 0.5	// top right
		]);
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_quad[1]);
		gl.bufferData(gl.ARRAY_BUFFER, quadTexCoords, gl.DYNAMIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_TEXTURE, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_TEXTURE);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	else if(keyPressed == 2) {
		quadTexCoords = new Float32Array([
			0.0, 2.0,	// top left
			0.0, 0.0,	// bottom left
			2.0, 0.0,	// bottom right
			2.0, 2.0	// top right
		]);
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_quad[1]);
		gl.bufferData(gl.ARRAY_BUFFER, quadTexCoords, gl.DYNAMIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_TEXTURE, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_TEXTURE);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	else if(keyPressed == 3) {
		quadTexCoords = new Float32Array([
			0.5, 0.5,	// top left
			0.5, 0.5,	// bottom left
			0.5, 0.5,	// bottom right
			0.5, 0.5	// top right
		]);
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_quad[1]);
		gl.bufferData(gl.ARRAY_BUFFER, quadTexCoords, gl.DYNAMIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_TEXTURE, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_TEXTURE);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [0.0, 0.0, -3.0]);
	mat4.multiply(ModelViewMatrix, ModelViewMatrix, TranslationMatrix);
	mat4.multiply(ModelViewProjectionMatrix, perspProjMatrix, ModelViewMatrix);
	gl.uniformMatrix4fv(MVPUniform, false, ModelViewProjectionMatrix);
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, smiley_texture);
	gl.uniform1i(TextureSamplerUniform, 0);
	gl.bindVertexArray(VAObj_quad);
		gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.bindVertexArray(null);

	gl.useProgram(null);

	// Similar to glFlush / swapBuffers / glXSwapBuffers / Game loop
	requestAnimationFrame(display, canvas);
}

// Function uninitialize
function uninitialize() {
	// Code
	// Delete Vertex Array Object
	if(VAObj_quad) {
		gl.deleteVertexArray(VAObj_quad);
		VAObj_quad = null;
	}

	// Delete Vertex Buffer Object
	if(VBObj_quad) {
		gl.deleteBuffer(VBObj_quad);
		VBObj_quad = null;
	}

	// Delete Textures
	if(smiley_texture) {
		gl.deleteTexture(smiley_texture);
		smiley_texture = null;
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
