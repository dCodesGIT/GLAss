// Interleaved array in WebGL
// By : Darshan Vikam
// Date : 06 July 2021

// Global variables
var canvas = null;
var gl = null;
var canvas_original_width = 0;
var canvas_original_height = 0;
var bFullscreen = false;
var angle = 0.0;
const radians = Math.PI / 180.0;

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
var VBObj_cube;

var MMatrixUniform, VMatrixUniform, PMatrixUniform;
var LAmbUniform, LDiffUniform, LSpecUniform, LPosUniform;
var KAmbUniform, KDiffUniform, KSpecUniform, KShineUniform;
var TextureSamplerUniform;
var InterleavedEnabledUniform;

var marble_texture;
var gbInterleavedEnabled = false;
var gbAnimationEnabled = false;

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
		case 65 :	// A or a
			gbAnimationEnabled = !gbAnimationEnabled;
			break;
		case 73 :	// I or i
			gbInterleavedEnabled = !gbInterleavedEnabled;
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
		"in vec3 vPosition;" +
		"in vec3 vColor, vNormal;" +
		"in vec2 vTexCoord;" +
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" +
		"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" +
		"uniform vec4 u_LPos;" +
		"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" +
		"uniform float u_KShine;" +
		"uniform bool u_interleavedEnabled;" +
		"out vec3 out_light;" +
		"out vec2 out_texCoord;" +
		"void main(void) {" +
			"out_light = vec3(0.0);" +
			"out_texCoord = vec2(0.0);" +
			"if(u_interleavedEnabled) {" +
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vec4(vPosition, 1.0);" +
				"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);" +
				"vec3 lightSource = normalize(vec3(u_LPos - eyeCoords));" +
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" +
				"vec3 viewVector = normalize(-eyeCoords.xyz);" +
				"vec3 ambient = u_LAmb * u_KAmb;" +
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0) * vColor;" +
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" +
				"out_light = ambient + diffuse + specular;" +
				"out_texCoord = vTexCoord;" +
			"}" +
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vec4(vPosition, 1.0);" +
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
		"in vec2 out_texCoord;" +
		"uniform sampler2D u_texture_sampler; " +
		"out vec4 FragColor;" +
		"void main(void) {" +
			"vec3 texColor = texture(u_texture_sampler, out_texCoord).rgb;" +
			"FragColor = vec4(out_light * texColor, 1.0);" +
			// "FragColor = vec4(1.0, 0.0, 0.0, 0.0);" +
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
	gl.bindAttribLocation(SPObj, WebGLMacros.DV_ATTRIB_NORMAL, "vNormal");
	gl.bindAttribLocation(SPObj, WebGLMacros.DV_ATTRIB_TEXTURE, "vTexCoord");
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
	TextureSamplerUniform = gl.getUniformLocation(SPObj, "u_texture_sampler");
	InterleavedEnabledUniform = gl.getUniformLocation(SPObj, "u_interleavedEnabled");

	// Initializing vertices, color, shader attibs, VAO, VBO
	var cube = new Float32Array([
		// Front face (Top left) - Vertices, Color(red), Normals, TexCoords
		-1.0, 1.0, 1.0,		1.0, 0.0, 0.0,		0.0, 0.0, 1.0,		0.0, 1.0,
		// Front face (Bottom left) - Vertices, Color(red), Normals, TexCoords
		-1.0, -1.0, 1.0,	1.0, 0.0, 0.0,		0.0, 0.0, 1.0,		0.0, 0.0,
		// Front face (Bottom right) - Vertices, Color(red), Normals, TexCoords
		1.0, -1.0, 1.0,		1.0, 0.0, 0.0,		0.0, 0.0, 1.0,		1.0, 0.0,
		// Front face (Top right) - Vertices, Color(red), Normals, TexCoords
		1.0, 1.0, 1.0,		1.0, 0.0, 0.0,		0.0, 0.0, 1.0,		1.0, 1.0,
		
		// Right face (Top left) - Vertices, Color(green), Normals, TexCoords
		1.0, 1.0, 1.0,		0.0, 1.0, 0.0,		1.0, 0.0, 0.0,		0.0, 1.0,
		// Right face (Botttom left) - Vertices, Color(green), Normals, TexCoords
		1.0, -1.0, 1.0,		0.0, 1.0, 0.0,		1.0, 0.0, 0.0,		0.0, 0.0,
		// Right face (Bottom right) - Vertices, Color(green), Normals, TexCoords
		1.0, -1.0, -1.0,	0.0, 1.0, 0.0,		1.0, 0.0, 0.0,		1.0, 0.0,
		// Right face (Top right) - Vertices, Color(green), Normals, TexCoords
		1.0, 1.0, -1.0,		0.0, 1.0, 0.0,		1.0, 0.0, 0.0,		1.0, 1.0,
		
		// Bottom face (Top left) - Vertices, Color(blue), Normals, TexCoords
		1.0, -1.0, 1.0,		0.0, 0.0, 1.0,		0.0, -1.0, 0.0,		0.0, 1.0,
		// Bottom face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
		-1.0, -1.0, 1.0,	0.0, 0.0, 1.0,		0.0, -1.0, 0.0,		0.0, 0.0,
		// Bottom face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
		-1.0, -1.0, -1.0,	0.0, 0.0, 1.0,		0.0, -1.0, 0.0,		1.0, 0.0,
		// Bottom face (Top right) - Vertices, Color(blue), Normals, TexCoords
		1.0, -1.0, -1.0,	0.0, 0.0, 1.0,		0.0, -1.0, 0.0,		1.0, 1.0,
		
		// Left face (Top left) - Vertices, Color(green), Normals, TexCoords
		-1.0, 1.0, -1.0,	0.0, 1.0, 0.0,		-1.0, 0.0, 0.0,		0.0, 1.0,
		// Left face (Bottom left) - Vertices, Color(green), Normals, TexCoords
		-1.0, -1.0, -1.0,	0.0, 1.0, 0.0,		-1.0, 0.0, 0.0,		0.0, 0.0,
		// Left face (Bottom right) - Vertices, Color(green), Normals, TexCoords
		-1.0, -1.0, 1.0,	0.0, 1.0, 0.0,		-1.0, 0.0, 0.0,		1.0, 0.0,
		// Left face (Top right) - Vertices, Color(green), Normals, TexCoords
		-1.0, 1.0, 1.0,		0.0, 1.0, 0.0,		-1.0, 0.0, 0.0,		1.0, 1.0,
		
		// Back face (Top left) - Vertices, Color(red), Normals, TexCoords
		1.0, 1.0, -1.0,		1.0, 0.0, 0.0,		0.0, 0.0, -1.0,		0.0, 1.0,
		// Back face (Bottom left) - Vertices, Color(red), Normals, TexCoords
		1.0, -1.0, -1.0,	1.0, 0.0, 0.0,		0.0, 0.0, -1.0,		0.0, 0.0,
		// Back face (Bottom right) - Vertices, Color(red), Normals, TexCoords
		-1.0, -1.0, -1.0,	1.0, 0.0, 0.0,		0.0, 0.0, -1.0,		1.0, 0.0,
		// Back face (Top right) - Vertices, Color(red), Normals, TexCoords
		-1.0, 1.0, -1.0,	1.0, 0.0, 0.0,		0.0, 0.0, -1.0,		1.0, 1.0,
		
		// Top face (Top left) - Vertices, Color(blue), Normals, TexCoords
		1.0, 1.0, 1.0,		0.0, 0.0, 1.0,		0.0, 1.0, 0.0,		0.0, 1.0,
		// Top face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
		1.0, 1.0, -1.0,		0.0, 0.0, 1.0,		0.0, 1.0, 0.0,		0.0, 0.0,
		// Top face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
		-1.0, 1.0, -1.0,	0.0, 0.0, 1.0,		0.0, 1.0, 0.0,		1.0, 0.0,
		// Top face (Top right) - Vertices, Color(blue), Normals, TexCoords
		-1.0, 1.0, 1.0,		0.0, 0.0, 1.0,		0.0, 1.0, 0.0,		1.0, 1.0,
	]);

	VAObj_cube = gl.createVertexArray();		// Cube
	gl.bindVertexArray(VAObj_cube);
		VBObj_cube = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, VBObj_cube);
		gl.bufferData(gl.ARRAY_BUFFER, cube, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_VERTEX, 3, gl.FLOAT, false, (3+3+3+2) * Float32Array.BYTES_PER_ELEMENT, 0);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_VERTEX);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_COLOR, 3, gl.FLOAT, false, (3+3+3+2) * Float32Array.BYTES_PER_ELEMENT, 3 * Float32Array.BYTES_PER_ELEMENT);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_COLOR);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_NORMAL, 3, gl.FLOAT, false, (3+3+3+2) * Float32Array.BYTES_PER_ELEMENT, 6 * Float32Array.BYTES_PER_ELEMENT);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_NORMAL);
		gl.vertexAttribPointer(WebGLMacros.DV_ATTRIB_TEXTURE, 2, gl.FLOAT, false, (3+3+3+2) * Float32Array.BYTES_PER_ELEMENT, 9 * Float32Array.BYTES_PER_ELEMENT);
		gl.enableVertexAttribArray(WebGLMacros.DV_ATTRIB_TEXTURE);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	gl.enable(gl.DEPTH_TEST);
	gl.clearDepth(1.0);
	gl.depthFunc(gl.LEQUAL);

	marble_texture = loadTexture(gl, "marble.png");

	gbAnimationEnabled = false;
	gbInterleavedEnabled = false;

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

	var MMatrix = mat4.create();
	var VMatrix = mat4.create();
	var PMatrix = perspProjMatrix;
	var TranslationMatrix = mat4.create();
	var RotationMatrix = mat4.create();

	gl.useProgram(SPObj);
	if(gbInterleavedEnabled) {
		gl.uniform1i(InterleavedEnabledUniform, true);
		gl.uniform3f(LAmbUniform, 0.0, 0.0, 0.0);
		gl.uniform3f(LDiffUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(LSpecUniform, 0.0, 0.0, 0.0);
		gl.uniform4f(LPosUniform, 0.0, 0.0, 2.0, 1.0);
		gl.uniform3f(KAmbUniform, 0.0, 0.0, 0.0);
		gl.uniform3f(KDiffUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(KSpecUniform, 1.0, 1.0, 1.0);
		gl.uniform1f(KShineUniform, 50.0);
	}
	else
		gl.uniform1i(InterleavedEnabledUniform, false);
	
	TranslationMatrix = mat4.create();
	mat4.translate(TranslationMatrix, TranslationMatrix, [0.0, 0.0, -5.0]);
	mat4.multiply(MMatrix, MMatrix, TranslationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, angle * radians, [1.0, 0.0, 0.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, angle * 2 * radians, [0.0, 1.0, 0.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);
	RotationMatrix = mat4.create();
	mat4.rotate(RotationMatrix, RotationMatrix, angle * 3 * radians, [0.0, 0.0, 1.0]);
	mat4.multiply(MMatrix, MMatrix, RotationMatrix);

	gl.uniformMatrix4fv(MMatrixUniform, false, MMatrix);
	gl.uniformMatrix4fv(VMatrixUniform, false, VMatrix);
	gl.uniformMatrix4fv(PMatrixUniform, false, PMatrix);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, marble_texture);
	gl.uniform1i(TextureSamplerUniform, 0);
	gl.bindVertexArray(VAObj_cube);
		gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
		gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	gl.bindVertexArray(null);

	gl.useProgram(null);

	if(gbAnimationEnabled)
		angle += 0.1;
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

	// Delete Textures
	if(marble_texture) {
		gl.deleteTexture(marble_texture);
		marble_texture = null;
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
