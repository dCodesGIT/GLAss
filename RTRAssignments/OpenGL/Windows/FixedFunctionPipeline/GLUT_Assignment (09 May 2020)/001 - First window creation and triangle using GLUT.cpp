// First code of Window creation and triangle using OpenGL - GLUT
// Date : 09 May 2020

#include <GL/freeglut.h>

bool bFullscreen = false;

int main(int argc, char **argv) {
	// Main of the code
	// Function declaration
	void initialize(void);
	void resize(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(400, 300);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("GLUT : My first window");

	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);
	glutMainLoop();

	return(0);
}

void initialize(void) {
	// Body of initialize()
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// clear screen with black color
}

void resize(int width, int height) {
	// Body of resize(int, int)
	if(height <= 0)
		height = 100;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void display(void) {
	// Body of display()
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glBegin(GL_TRIANGLES);
	glColor3f(2.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 2.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 2.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	// Body of keyboard(unsigned char, int, int)
	switch(key) {
		case 27 :
		case 'x' :
		case 'X' :
			glutLeaveMainLoop();
			break;
		case 'f' :
		case 'F' :
			if(bFullscreen == false) {
				glutFullScreen();
				bFullscreen = true;
			}
			else {
				glutLeaveFullScreen();
				bFullscreen = false;
			}
			break;
		default :
			break;
	}
}

void mouse(int button, int state, int x, int y) {
	//  Body of mouse(int, int, int, int)
	switch(button) {
		case GLUT_LEFT_BUTTON :
			break;
		case GLUT_RIGHT_BUTTON :
			glutLeaveMainLoop();
			break;
		default :
			break;
	}
}

void uninitialize(void) {
	// Body of uninitialize()
}
