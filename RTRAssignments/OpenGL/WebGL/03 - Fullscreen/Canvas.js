// Fullscreen in WebGL
// By : Darshan Vikam
// Date : 15 May 2021

// Global variables
var canvas = null;
var context = null;

// Main function - entry point function 
function main() {
	// Get Canvas from DOM
	canvas = document.getElementById("DV");
	if(!canvas)
		console.log("Canvas not found \n");
	else
		console.log("Canvas obtained successfully.. \n");

	// Get canvas width and height
	console.log("\n Canvas Width = " + canvas.width + "\n Canvas Height = " + canvas.height);

	// Get drawing context from the canvas
	context = canvas.getContext("2d");
	if(!context)
		console.log("Context not found \n");
	else
		console.log("Context obtained successfully.. \n");

	// Paint background by black color
	context.fillStyle = "black";
	context.fillRect(0, 0, canvas.width, canvas.height);

	drawText("Hello World !!!");

	// Setting Event listeners (for event handling)
	window.addEventListener("keydown", keyDown, false);	// window is an in-built variable
	window.addEventListener("click", mouseDown, false);
}

// Function which writes text on canvas
function drawText(text) {
	// Paint background by black color
	context.fillStyle = "black";
	context.fillRect(0, 0, canvas.width, canvas.height);

	// Center the upcoming text
	context.textAlign = "center";
	context.textBaseline = "middle";

	// Setting font size, style and color
	context.font = "48px sans-serif";
	context.fillStyle = "green";

	// Actual displaying of text
	context.fillText(text, canvas.width/2, canvas.height/2);
}

// Function to toggle fullscreen - multi browser complient
function toggleFullscreen() {
	// Variable declaration
	var fullscreen_element = document.fullscreenElement ||			// For Google Chrome, Opera
				 document.webkitFullscreenElement ||		// For Apple's - Safari
				 document.mozFullScreenElement ||		// For Mozilla firefox
				 document.msFullscreenElement ||		// For MicroSoft's - Internet Explorer
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
	}
}

// Event listener / handler - for Keyboard event
function keyDown(event) {
	// message box in web
	switch(event.keyCode) {
		case 70 :	// f or F
			toggleFullscreen();
			drawText("Hello WebGL !!!");
			break;
		default :
			break;
	}
}

// Event handling - for mouse button events
function mouseDown() {
	// message box in web
}
