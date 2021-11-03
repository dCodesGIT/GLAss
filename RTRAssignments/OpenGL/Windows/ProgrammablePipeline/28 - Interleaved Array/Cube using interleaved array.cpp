// Program to show interleaved array - Cube with lights, textures and colors
// By : Darshan Vikam
// Date : 0 July 2021

// Global header files
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include "../Include/vmath.h"
#include "../Icon/WinIcon.h"
#include "WinTexture.h"

// Library linking
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

// Call back function - WndProc()
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global macro definitions
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Namespace declaration
using namespace vmath;

// Global enum declaration
enum {
	DV_ATTRIBUTE_POSITION = 0,
	DV_ATTRIBUTE_COLOR,
	DV_ATTRIBUTE_NORMAL,
	DV_ATTRIBUTE_TEXTURE0,
};

// Global variable declaration
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE *gfp = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbFullscreen = false;
bool gbEnableAnimation = false;
bool gbEnableInterleaved = false;

GLfloat gGLfangle = 0.0f;

GLuint gVSObj, gFSObj, gSPObj;
GLuint gVAObj_Cube;
GLuint gVBObj_Cube;

GLuint MMatrixUniform, VMatrixUniform, PMatrixUniform;
GLuint LAmbUniform, LDiffUniform, LSpecUniform, LPosUniform;
GLuint KAmbUniform, KDiffUniform, KSpecUniform, KShineUniform;
GLuint InterleavedEnabledUniform;

GLuint TextureSamplerUniform;
GLuint marble_texture;

mat4 gPerspProjMatrix;	// Matrix of 4x4

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declarations
	void Initialize(void);
	void Display(void);
	void Update(void);
	void Uninitialize(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("CubeUsingInterleavedArrayInPP");
	bool bDone = false;

	// Code
	if(fopen_s(&gfp, "Log.txt", "w") != 0) {
		MessageBox(NULL, TEXT("Log.txt file cannot be created.\n Exitting..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
		fprintf(gfp, "Log file created successfully...\n");

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Cube using interleaved array"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	Initialize();

	while(bDone == false) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT)
				bDone = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			Display();
			Update();
		}
	}

	return((int)msg.wParam);
}

// Call back function definition - WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Function declaration
	void Resize(int, int);
	void ToggleFullscreen(void);
	void Uninitialize(void);

	// Code
	switch(iMsg) {
		case WM_ACTIVATE :
			if(HIWORD(wParam) == 0)
				gbActiveWindow = true;
			else
				gbActiveWindow = false;
			break;

		case WM_ERASEBKGND :
			return(0);

		case WM_SIZE :
			Resize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_KEYDOWN :
			switch(wParam) {
				case VK_ESCAPE :
					if(MessageBox(ghwnd, TEXT("Do you want really want to quit?"), TEXT("Quit"), MB_YESNO) == IDYES)
						DestroyWindow(ghwnd);
					break;
				case 0x46 :
				case 0x66 :
					ToggleFullscreen();
					if(gbFullscreen == false)
						gbFullscreen = true;
					else
						gbFullscreen = false;
					break;
				default :
					break;
			}
			break;

		case WM_CHAR :
			switch(wParam) {
				case 'a' :
				case 'A' :
					gbEnableAnimation = !gbEnableAnimation;
					break;
				case 'i' :
				case 'I' :
					gbEnableInterleaved = !gbEnableInterleaved;
					break;
				default :
					break;
			}
			break;

		case WM_CLOSE :
			if(MessageBox(ghwnd, TEXT("Do you want really want to quit?"), TEXT("Quit"), MB_YESNO) == IDYES)
				DestroyWindow(ghwnd);
			else
				return true;
			break;

		case WM_DESTROY :
			Uninitialize();
			PostQuitMessage(0);
			break;

		default :
			break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

// User defined function definitions
void ToggleFullscreen(void) {
	// Variable declaration
	MONITORINFO mi;

	// Code
	if(gbFullscreen == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW) {
			mi = {sizeof(MONITORINFO)};
			if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void Initialize(void) {
	// Function declaration
	void Uninitialize(void);
	void Resize(int, int);
	void ShaderErrorCheck(GLuint, char*);			// ShaderErrorCheck(GLuint shader_object, char* ["COMPILE"/"LINK"]);
	bool loadGLTexture(GLuint *, TCHAR []);

	// Variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	FILE *fp_OGL_info = NULL;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0)
		DestroyWindow(ghwnd);

	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
		DestroyWindow(ghwnd);

	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL)
		DestroyWindow(ghwnd);

	if(wglMakeCurrent(ghdc, ghrc) == false)
		DestroyWindow(ghwnd);

	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
		DestroyWindow(ghwnd);

	// OpenGL related log entry
	fopen_s(&fp_OGL_info, "OpenGL_info.txt", "w");
	if(fp_OGL_info == NULL)
		MessageBox(ghwnd, TEXT("Unable to open file to write OpenGL related information"), NULL, NULL);
	fprintf(fp_OGL_info, "*** OpenGL Information ***\n\n");
	fprintf(fp_OGL_info, "*** OpenGL related basic information ***\n");
	fprintf(fp_OGL_info, "OpenGL Vendor Company : %s\n", glGetString(GL_VENDOR));
	fprintf(fp_OGL_info, "OpenGL Renderer(Graphics card company) : %s\n", glGetString(GL_RENDERER));
	fprintf(fp_OGL_info, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(fp_OGL_info, "Graphics Library Shading Language(GLSL) Version : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(fp_OGL_info, "*** OpenGL supported/related extentions ***\n");
	// OpenGL supported/related Extensions
	GLint numExts;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);
	for(int i = 0; i < numExts; i++)
		fprintf(fp_OGL_info, "%d. %s\n", i+1, glGetStringi(GL_EXTENSIONS, i));
	fclose(fp_OGL_info);
	fp_OGL_info = NULL;

	// Vertex Shader
	gVSObj = glCreateShader(GL_VERTEX_SHADER);		// Create shader
	const GLchar *VSSrcCode = 				// Source code of shader
		"#version 450 core \n" \
		"in vec4 vPosition;" \
		"in vec3 vColor, vNormal;" \
		"in vec2 vTexCoord;" \
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
		"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" \
		"uniform vec4 u_LPos;" \
		"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
		"uniform float u_KShine;" \
		"uniform bool u_InterleavedEnabled;" \
		"out vec3 out_light;" \
		"out vec2 out_texCoord;" \
		"void main(void) {" \
			"out_light = vec3(0.0);" \
			"out_texCoord = vec2(0.0);" \
			"if(u_InterleavedEnabled) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);" \
				"vec3 lightSource = normalize(vec3(u_LPos - eyeCoords));" \
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" \
				"vec3 viewVector = normalize(-eyeCoords.xyz);" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0) * vColor;" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
				
				"out_light = ambient + diffuse + specular;" \
				"out_texCoord = vTexCoord;" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);				// Compile Shader
	ShaderErrorCheck(gVSObj, "COMPILE");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 450 core \n" \
		"in vec3 out_light;" \
		"in vec2 out_texCoord;" \
		"uniform sampler2D u_textureSampler;"
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 out_texture = texture(u_textureSampler, out_texCoord).rgb;" \
			"FragColor = vec4(out_light * out_texture, 1.0);" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);				// Compile Shader
	ShaderErrorCheck(gFSObj, "COMPILE");

	// Shader program
	gSPObj = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj, gVSObj);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj, gFSObj);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_POSITION, "vPosition");	// Binding of shader program with enum declared - for position(coordinates)
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_COLOR, "vColor");		// Binding of shader program with enum declared - for color
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_TEXTURE0, "vTexCoord");	// Binding of shader program with enum declared - for texture coordinates
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_NORMAL, "vNormal");		// Binding of shader program with enum declared - for normal
	glLinkProgram(gSPObj);
	ShaderErrorCheck(gSPObj, "LINK");

	// get MVP uniform location
	MMatrixUniform = glGetUniformLocation(gSPObj, "u_MMatrix");
	VMatrixUniform = glGetUniformLocation(gSPObj, "u_VMatrix");
	PMatrixUniform = glGetUniformLocation(gSPObj, "u_PMatrix");
	LAmbUniform = glGetUniformLocation(gSPObj, "u_LAmb");
	LDiffUniform = glGetUniformLocation(gSPObj, "u_LDiff");
	LSpecUniform = glGetUniformLocation(gSPObj, "u_LSpec");
	LPosUniform = glGetUniformLocation(gSPObj, "u_LPos");
	KAmbUniform = glGetUniformLocation(gSPObj, "u_KAmb");
	KDiffUniform = glGetUniformLocation(gSPObj, "u_KDiff");
	KSpecUniform = glGetUniformLocation(gSPObj, "u_KSpec");
	KShineUniform = glGetUniformLocation(gSPObj, "u_KShine");
	TextureSamplerUniform = glGetUniformLocation(gSPObj, "u_textureSampler");
	InterleavedEnabledUniform = glGetUniformLocation(gSPObj, "u_InterleavedEnabled");

	// other variable initialization
	const GLfloat Cube[] = {
		// Front face (Top left) - Vertices, Color(red), Normals, TexCoords
		-1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		// Front face (Bottom left) - Vertices, Color(red), Normals, TexCoords
		-1.0f, -1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		// Front face (Bottom right) - Vertices, Color(red), Normals, TexCoords
		1.0f, -1.0, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
		// Front face (Top right) - Vertices, Color(red), Normals, TexCoords
		1.0f, 1.0, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		
		// Right face (Top left) - Vertices, Color(green), Normals, TexCoords
		1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		// Right face (Botttom left) - Vertices, Color(green), Normals, TexCoords
		1.0f, -1.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		// Right face (Bottom right) - Vertices, Color(green), Normals, TexCoords
		1.0f, -1.0, -1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		// Right face (Top right) - Vertices, Color(green), Normals, TexCoords
		1.0f, 1.0, -1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		
		// Bottom face (Top left) - Vertices, Color(blue), Normals, TexCoords
		1.0f, -1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
		// Bottom face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, -1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
		// Bottom face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, -1.0, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,
		// Bottom face (Top right) - Vertices, Color(blue), Normals, TexCoords
		1.0f, -1.0, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
		
		// Left face (Top left) - Vertices, Color(green), Normals, TexCoords
		-1.0f, 1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		// Left face (Bottom left) - Vertices, Color(green), Normals, TexCoords
		-1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		// Left face (Bottom right) - Vertices, Color(green), Normals, TexCoords
		-1.0f, -1.0, 1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		// Left face (Top right) - Vertices, Color(green), Normals, TexCoords
		-1.0f, 1.0, 1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		
		// Back face (Top left) - Vertices, Color(red), Normals, TexCoords
		1.0f, 1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
		// Back face (Bottom left) - Vertices, Color(red), Normals, TexCoords
		1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
		// Back face (Bottom right) - Vertices, Color(red), Normals, TexCoords
		-1.0f, -1.0, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
		// Back face (Top right) - Vertices, Color(red), Normals, TexCoords
		-1.0f, 1.0, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
		
		// Top face (Top left) - Vertices, Color(blue), Normals, TexCoords
		1.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		// Top face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
		1.0f, 1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		// Top face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, 1.0, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		// Top face (Top right) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, 1.0, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	};

	// For Cube
	glGenVertexArrays(1, &gVAObj_Cube);
	glBindVertexArray(gVAObj_Cube);
		glGenBuffers(1, &gVBObj_Cube);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_Cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), Cube, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)0);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glVertexAttribPointer(DV_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIBUTE_NORMAL);
		glVertexAttribPointer(DV_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(9 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIBUTE_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Depth related OpenGL code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Texture loading
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&marble_texture, MAKEINTRESOURCE(MARBLE_BITMAP));

	gbEnableInterleaved = false;
	gbEnableAnimation = false;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	gPerspProjMatrix = mat4::identity();

	// Warm up call to Resize()
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void ShaderErrorCheck(GLuint shaderObject, char *shaderOpr) {	// Error checking after shader compilation
	// Variable declaration
	GLint iErrorLen = 0;
	GLint iShaderStatus = 0;
	char *szError = NULL;

	// Code
	if(strcmp(shaderOpr, "COMPILE") == 0)
		glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &iShaderStatus);
	else if(strcmp(shaderOpr, "LINK") == 0)
		glGetShaderiv(shaderObject, GL_LINK_STATUS, &iShaderStatus);
	else {
		fprintf(gfp, "Invalid second parameter in ShaderErrorCheck()");
		return;
	}
	if(iShaderStatus == GL_FALSE) {
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &iErrorLen);
		if(iErrorLen > 0) {
			szError = (char *)malloc(iErrorLen);
			if(szError != NULL) {
				GLsizei written;
				glGetShaderInfoLog(shaderObject, iErrorLen, &written, szError);
				if(strcmp(shaderOpr, "COMPILE") == 0)
					fprintf(gfp, "Shader Compilation Error log : \n");
				else if(strcmp(shaderOpr, "LINK") == 0)
					fprintf(gfp, "Shader linking Error log : \n");
				fprintf(gfp, "%s \n", szError);
				free(szError);
				szError = NULL;
				DestroyWindow(ghwnd);
			}
		}
	}
}

bool loadGLTexture(GLuint *texture, TCHAR resourceID[]) {
	// Variable declaration
	bool bResult = false;
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	// Code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), resourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if(hBitmap) {
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		// Actual code of texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	return bResult;
}

void Resize(int width, int height) {
	// Code
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gPerspProjMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void Display(void) {
	// Variable declaration
	mat4 modelMatrix, viewMatrix, projectionMatrix;
	mat4 translateMatrix, rotationMatrix;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Starting of OpenGL shading program
	glUseProgram(gSPObj);

	if(gbEnableInterleaved) {
		glUniform1i(InterleavedEnabledUniform, true);
		glUniform3f(LAmbUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(LDiffUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(LSpecUniform, 0.0f, 0.0f, 0.0f);
		glUniform4f(LPosUniform, 0.0f, 0.0f, 2.0f, 1.0f);
		glUniform3f(KAmbUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(KDiffUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(KSpecUniform, 1.0f, 1.0f, 1.0f);
		glUniform1f(KShineUniform, 50.0f);
	}
	else
		glUniform1i(InterleavedEnabledUniform, false);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = gPerspProjMatrix;
	translateMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -5.0f);
	rotationMatrix = rotate(gGLfangle, 1.0f, 0.0f, 0.0f);
	modelMatrix = translateMatrix * rotationMatrix;
	rotationMatrix = rotate(gGLfangle, 0.0f, 1.0f, 0.0f);
	modelMatrix *= rotationMatrix;
	rotationMatrix = rotate(gGLfangle, 0.0f, 0.0f, 1.0f);
	modelMatrix *= rotationMatrix;

	glUniformMatrix4fv(MMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(VMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(PMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// Texture enabling and sending it to shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marble_texture);
	glUniform1i(TextureSamplerUniform, 0);
	
	// OpenGL Drawing
	glBindVertexArray(gVAObj_Cube);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);

	// End of OpenGL shading program
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void Update(void) {
	// Code
	if(gbEnableAnimation == true)
		gGLfangle += 0.02f;
	if(gGLfangle >= 360.0f)
		gGLfangle = 0.0f;
}

void Uninitialize(void) {
	// Code
	if(gbFullscreen == true)
		ToggleFullscreen();

	// Destroy Vertex Array Object
	if(gVAObj_Cube) {
		glDeleteVertexArrays(1, &gVAObj_Cube);
		gVAObj_Cube = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_Cube) {
		glDeleteBuffers(1, &gVBObj_Cube);
		gVBObj_Cube = 0;
	}

	// Delete textures
	glDeleteTextures(1, &marble_texture);

	// Detach shaders
	glDetachShader(gSPObj, gVSObj);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj, gFSObj);		// Detach fragment shader from final shader program

	// Delete shaders
	if(gVSObj != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj);
		gVSObj = 0;
	}
	if(gFSObj != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj);
		gFSObj = 0;
	}
	if(gSPObj != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj);
		gSPObj = 0;
	}

	// Unlink shader program
	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);

	if(ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if(ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if(gfp) {
		fprintf(gfp, "Closing Log file successfully..\n");
		fclose(gfp);
		gfp = NULL;
	}
}
