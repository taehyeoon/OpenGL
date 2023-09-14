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
int mouseClicked;
int screenWidth = 480;
int screenHeight = 480;

float positions[100];
int VertexNum = 0;

int clickCount = 0;
float radius;
const double PI = 3.1415926;

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

void renderScene(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);
	//Let's draw something here

	if (VertexNum == 2) {
		cout << "Draw quater circle" << endl;

		float targetVertices[22];

		for (int i = 0; i < 11; i++) {
			targetVertices[i*2] = radius * cos(9*i * PI / 180) + positions[0]; // x
			targetVertices[i*2+1] = radius * sin(9*i * PI / 180) + positions[1]; // y
		}
		GLuint VBO;
		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(targetVertices), targetVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_LINE_STRIP, 0, 11);
	}
	//else {
	//	GLuint VBO;
	//	glGenVertexArrays(1, &VBO);
	//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//	glEnableVertexAttribArray(0);

	//	glDrawArrays(GL_LINE_STRIP, 0, VertexNum);
	//}

	//define the size of point and draw a point.
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	//Double buffer
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


pair<float, float> nomalizedPosByLeftTop(int x, int y) {
	pair<float, float> result;
	result.first = (2.0f * x / screenWidth) - 1;
	result.second = 1 - (2.0f * y / screenHeight);

	return result;
}

void mousePressed(int btn, int state, int x, int y) {

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouseClicked = true;
		cout << x << " , " << y << endl;
		pair<float, float> normalPos = nomalizedPosByLeftTop(x, y);
		cout << normalPos.first << " , " << normalPos.second << "\n" << endl;

		if (VertexNum >= 2) return;

		positions[VertexNum*2] = normalPos.first;
		positions[VertexNum*2+1] = normalPos.second;
		VertexNum++;

		if (VertexNum == 2) {
			float x1 = positions[0];
			float y1 = positions[1];
			float x2 = positions[2];
			float y2 = positions[3];

			radius = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
			cout << "radius : ";
			cout << radius << endl;
		}
	}
	else {
		mouseClicked = false;
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
	glutInitWindowSize(screenWidth, screenHeight);
	//This is used to define the name of the window.
	glutCreateWindow("Simple OpenGL Window");

	//call initization function
	init();

	//float positions[] = {
	//	-0.2f, -0.8f,
	//	0.8f, 0.2f,
	//};

	glutMouseFunc(mousePressed);
	glutKeyboardFunc(keyboardPressed);

	//1.
	//Generate VAO
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenVertexArrays(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//3. 
	GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");

	glUseProgram(programID);

	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VAO);

	return 1;
}

