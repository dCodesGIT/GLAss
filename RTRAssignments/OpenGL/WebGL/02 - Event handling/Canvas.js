// Event Handling in WebGL
// By : Darshan Vikam
// Date : 15 May 2021

// Main function - entry point function 
function main() {
	// Get Canvas from DOM
	var canvas = document.getElementById("DV");
	if(!canvas)
		console.log("Canvas not found \n");
	else
		console.log("Canvas obtained successfully.. \n");

	// Get canvas width and height
	console.log("\n Canvas Width = " + canvas.width + "\n Canvas Height = " + canvas.height);

	// Get drawing context from the canvas
	var context = canvas.getContext("2d");
	if(!context)
		console.log("Context not found \n");
	else
		console.log("Context obtained successfully.. \n");

	// Paint background by black color
	context.fillStyle = "black";
	context.fillRect(0, 0, canvas.width, canvas.height);

	// Center the upcoming text
	context.textAlign = "center";		// Horizontal centering
	context.textBaseline = "middle";	// Vertical centering

	// Setting font size, style and color
	context.font = "48px sans-serif";
	context.fillStyle = "green";

	// Declare string to be displayed
	var str = "Hello World !!!";

	// Actual displaying of text
	context.fillText(str, canvas.width/2, canvas.height/2);

	// Setting Event listeners (for event handling)
	window.addEventListener("keydown", keyDown, false);	// window is an in-built variable
	window.addEventListener("click", mouseDown, false);
}

// Event listener / handler - for Keyboard event
function keyDown(event) {
	// message box in web
	alert( "Key Pressed" );
}

// Event handling - for mouse button events
function mouseDown() {
	// message box in web
	alert("Mouse key pressed");
}
