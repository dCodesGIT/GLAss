// To demonstrate escape sequences

#include <stdio.h>
int main() {
	// Code
	printf("\n Going to the next line using \\n escape sequence.\n");
	printf("\n Demonstrating \t horizontal \t tab \t using \t \\t escape sequence.");
	printf("\n \"This is a double quoted output\" done using \\\" \\\" escape sequence.");
	printf("\n \'This is a single quoted output\' done using \\\' \\\' escape sequence.");
	printf("\n BACKSPACE turned to BACKSPACE\b using \\b escape sequence.");

	printf("\n \r Demonstrating Carriage return using \\r escape sequence.");
	printf("\n Demonstrating \r Carriage return using \\r escape sequence.");
	printf("\n Demonstrating Carriage \r return using \\r escape sequence.");

	printf("\n Demonstrating \x41 using \\xhh escape sequence.");
	printf("\n Demonstrating \102 using \\ooo escape sequence.");

	printf("\n\n");
	return 0;
}