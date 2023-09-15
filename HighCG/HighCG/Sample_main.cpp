#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cmath>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

using namespace std;

struct Vector2
{
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 1080;
const double PI = 3.1415926;
const int TESSELLATION_DELATA = 1;
const int VERTEX_SIZE = 3;
const int COLOR_SIZE = 4;
const int ONE_VERTEX_DATA_SIZE = VERTEX_SIZE + COLOR_SIZE;

bool inputComplete;
int tessellation = 2;
int verticesNum;
double radius;
vector<float> verticesData;
Vector2 centerPos;
GLuint programID;



// Graphic
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
void init();
void renderScene(void);

// Calculate
void addAllCircleVertices();
void bindCircleData();

// Input
void mousePressed(int btn, int state, int x, int y);
void keyboardPressed(unsigned char key, int x, int y);
Vector2 nomalizedPosByLeftTop(int x, int y);

int main(int argc, char **argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);

	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(400, 0);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	//This is used to define the name of the window.
	glutCreateWindow("Simple OpenGL Window");

	//call initization function
	init();

	glutMouseFunc(mousePressed);
	glutKeyboardFunc(keyboardPressed);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	GLuint VBO;
	glGenVertexArrays(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");

	glUseProgram(programID);

	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VAO);

	return 1;
}



void renderScene(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);

	if (inputComplete) {
		cout << "Draw quater circle" << endl;
		cout << "verticesNum" << verticesNum << endl;
		glDrawArrays(GL_LINE_LOOP, 0, verticesNum);
	}

	glutSwapBuffers();
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if(VertexShaderStream.is_open())
	{
		string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> VertexShaderErrorMessage(max(InfoLogLength, int(1)));
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if(FragmentShaderStream.is_open())
	{
		string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> FragmentShaderErrorMessage(max(InfoLogLength, int(1)));
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	//Link the program
	fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}

void addAllCircleVertices() {
	
	int switchingPointLastIdx = (verticesData.size() / VERTEX_SIZE) - 2;
	float x, y, z;

	// Add Left quater circle vertices
	for (int i = switchingPointLastIdx; i >= 0; i--) {
		x = verticesData.at(VERTEX_SIZE * i);
		y = verticesData.at(VERTEX_SIZE * i + 1);
		z = 0;

		verticesData.push_back(-1 * x);
		verticesData.push_back(y);
		verticesData.push_back(0);
	}

	// Add bottom half circle vertices
	switchingPointLastIdx = (verticesData.size() / VERTEX_SIZE) - 2;
	for (int i = switchingPointLastIdx; i >= 1; i--) {
		x = verticesData.at(VERTEX_SIZE * i);
		y = verticesData.at(VERTEX_SIZE * i + 1);
		z = 0;

		verticesData.push_back(x);
		verticesData.push_back(-1 * y);
		verticesData.push_back(z);
	}

	// Only include vertices Data
	verticesNum = verticesData.size() / VERTEX_SIZE;

	// Random Color
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<mt19937::result_type> dist1000(1, 1000); // distribution in range [1, 6]

	float r = dist1000(rng) / 1000.0f;
	float g = dist1000(rng) / 1000.0f;
	float b = dist1000(rng) / 1000.0f;
	vector<float> lineColor = { r, g, b, 1 };
	vector<float>::iterator it;
	it = verticesData.begin();
	it += VERTEX_SIZE;

	// Insert color information between the location information of the point
	for (int i = 0; i < verticesNum - 1; i++) {
		it = verticesData.insert(it, lineColor.begin(), lineColor.end());
		it += ONE_VERTEX_DATA_SIZE;
	}
	verticesData.insert(it, lineColor.begin(), lineColor.end());

	// Just for debug
	int idx = 0;
	for (it = verticesData.begin(); it != verticesData.end(); it++) {
		cout << *it << ", ";
		idx += 1;
		if (idx == ONE_VERTEX_DATA_SIZE) {
			idx = 0;
			cout << endl;
		}
	}
}

void bindCircleData() {

	verticesData.clear();
	cout << "current Tessllation : " << tessellation << endl;

	for (int i = 0; i < tessellation + 1; i++) {
		verticesData.push_back(radius * cos(90.0 / tessellation * i * PI / 180.0));
		verticesData.push_back(radius * sin(90.0 / tessellation * i * PI / 180.0));
		verticesData.push_back(0);
	}

	addAllCircleVertices();

	float* targetVertices = new float[verticesData.size()];
	for (unsigned int i = 0; i < verticesData.size(); i++) {
		targetVertices[i] = verticesData.at(i);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesData.size(), targetVertices, GL_STATIC_DRAW);
	
	// Position
	glVertexAttribPointer(0, VERTEX_SIZE, 
		GL_FLOAT, GL_FALSE, 
		ONE_VERTEX_DATA_SIZE * sizeof(float), 
		(void*)0);
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(1, COLOR_SIZE, 
		GL_FLOAT, GL_FALSE, 
		ONE_VERTEX_DATA_SIZE * sizeof(float), 
		(void*)(VERTEX_SIZE * sizeof(float)));
	glEnableVertexAttribArray(1);

	delete[] targetVertices;
}

void init()
{
    //initilize the glew and check the errors.
    GLenum res = glewInit();
    if(res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
    }

	//select the background color
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void mousePressed(int btn, int state, int x, int y) {
	
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		if (inputComplete) return;

		// Normalize clicked position to range[-1,1]
		Vector2 normalizedPos = nomalizedPosByLeftTop(x, y);
		cout << x << " , " << y << " -> " << normalizedPos.x << " , " << normalizedPos.y << endl;;

		// Save center position
		if (centerPos.x == 0 && centerPos.y == 0) {
			centerPos = normalizedPos;
			return;
		}

		// Calculate radius
		radius = sqrt(pow(centerPos.x - normalizedPos.x, 2) + pow(centerPos.y - normalizedPos.y, 2));
		cout << "radius : ";
		cout << radius << endl;

		// Calculate Other points
		bindCircleData();

		// Transfer center pos to vertex shader
		GLint centerPosLocation = glGetUniformLocation(programID, "CenterPos");
		glUniform3f(centerPosLocation, centerPos.x, centerPos.y, 0);

		inputComplete = true;
	}

	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

		if (!inputComplete) return;

		tessellation += TESSELLATION_DELATA;
		bindCircleData();
	}
}

void keyboardPressed(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
}

Vector2 nomalizedPosByLeftTop(int x, int y) {
	Vector2 result;
	result.x = (2.0f * x / SCREEN_WIDTH) - 1;
	result.y = 1 - (2.0f * y / SCREEN_HEIGHT);

	return result;
}