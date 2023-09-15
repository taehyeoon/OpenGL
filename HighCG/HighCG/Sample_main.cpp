#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

using namespace std;

const int SCREENWIDTH = 480;
const int SCREENHEIGHT = 480;
const int Tessellation = 30;
const double PI = 3.1415926;

float positions[100];
vector<float> drawVertices;
int VertexNum = 0;
float radius;

GLuint programID;

struct Vector2
{
	float x;
	float y;
};

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

void calcVertices() {
	
	int switchingPointLastIdx = Tessellation - 1;

	// start idx 1 (except start point)
	for (int i = switchingPointLastIdx; i >= 0; i--) {
		float x = drawVertices.at(2 * i);
		float y = drawVertices.at(2 * i + 1);
	
		drawVertices.push_back(-1 * x);
		drawVertices.push_back(y);
	}
}

void renderScene(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);

	if (VertexNum == 2) {
		cout << "Draw quater circle" << endl;

		for (int i = 0; i < Tessellation + 1; i++) {
			drawVertices.push_back(radius * cos(90.0 / Tessellation * i * PI / 180.0));
			drawVertices.push_back(radius * sin(90.0 / Tessellation * i * PI / 180.0));
		}
		calcVertices();
		cout << "size" << drawVertices.size() << endl;

		float* targetVertices = new float[drawVertices.size()];
		for (int i = 0; i < drawVertices.size(); i++) {
			targetVertices[i] = drawVertices.at(i);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLsizeiptr) * 4 * (drawVertices.size() / 2), targetVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		delete[] targetVertices;

		glDrawArrays(GL_LINE_STRIP, 0, drawVertices.size() / 2);
	}

	glutSwapBuffers();
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

Vector2 nomalizedPosByLeftTop(int x, int y) {
	Vector2 result;
	result.x = (2.0f * x / SCREENWIDTH) - 1;
	result.y = 1 - (2.0f * y / SCREENHEIGHT);

	return result;
}

void mousePressed(int btn, int state, int x, int y) {
	
	if (VertexNum >= 2) return;

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Normalize clicked position to range[-1,1]
		cout << x << " , " << y << endl;
		Vector2 normalizedPos = nomalizedPosByLeftTop(x, y);
		cout << normalizedPos.x << " , " << normalizedPos.y << endl;

		// Save clicked position to positions array 
		positions[VertexNum*2] = normalizedPos.x;
		positions[VertexNum*2+1] = normalizedPos.y;
		VertexNum++;

		// if second click
		if (VertexNum == 2) {
			Vector2 center = { positions[0], positions[1] };
			Vector2 secondPoint = { positions[2], positions[3] };

			// Calculate radius
			radius = sqrt(pow(center.x - secondPoint.x, 2) + pow(center.y - secondPoint.y, 2));
			cout << "radius : ";
			cout << radius << endl;

			// Transfer center pos to vertex shader
			GLint centerPosLocation = glGetUniformLocation(programID, "CenterPos");
			glUniform3f(centerPosLocation, center.x, center.y, 0);
		}
	}
}

void keyboardPressed(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
}

int main(int argc, char **argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);

	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT);
	
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

