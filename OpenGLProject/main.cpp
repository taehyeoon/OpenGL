#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cmath>
#include <random>

#ifdef WINDOWS
#include <GL/glew.h>
#else
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include "glm/glm.hpp"
#endif
#include <GLut/glut.h>

using namespace std;
using namespace glm;

// Screen
const int SCREEN_WIDTH = 540;
const int SCREEN_HEIGHT = 540;

// Tessellation
const int TESSEL_DELATA = 1;
const int TESSEL_MAX = 255;
const int TESSEL_MIN = 1;
int tessel = 10;

const double PI = 3.1415926;
const int VERTEX_SIZE = 3;
const int COLOR_SIZE = 4;
const int ONE_VERTEX_DATA_SIZE = VERTEX_SIZE + COLOR_SIZE;

bool isInputComplete;
vector<vec2> controlPoints;
GLuint programID;
GLint centerPosLocation;

// Graphic
GLuint CreateShader(int shaderType, const char* file_path);
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path,
    const char* tess_control_file_path, const char* tess_eval_file_path);
void init();
void renderScene(void);

// Input
void mousePressed(int btn, int state, int x, int y);
void keyboardPressed(unsigned char key, int x, int y);
vec2 normalizedPosByLeftTop(int x, int y);

int main(int argc, char **argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
#ifdef WINDOWS  // 윈도우즈에서 컴파일 할때는 아래를 포함
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA);
#endif
    
    //These two functions are used to define the position and size of the window.
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    //This is used to define the name of the window.
    glutCreateWindow("Simple OpenGL Window");
    
    //call initization function
    init();

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    programID = LoadShaders("VertexShader.txt",
                            "FragmentShader.txt",
                            "GeometryShader.txt",
                            "TessControlShader.txt",
                            "TessEvalShader.txt");
    glUseProgram(programID);

    // Position
    glVertexAttribPointer(0, 3,
        GL_FLOAT, GL_FALSE,
        3 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    // Patch data
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // Tessellation
    GLuint tesselationID = glGetUniformLocation(programID, "u_tessellation");
    glUniform1i(tesselationID, tessel);
    cout << "Current Tessellation : " << tessel << endl;

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

    if (isInputComplete) {
        for (int i = 0; i < controlPoints.size() - 3; i++) {
            glDrawArrays(GL_PATCHES, i, 4);
        }
    }

    glutSwapBuffers();
}

GLuint CreateShader(int shaderType, const char* file_path) 
{
    GLuint ShaderID = glCreateShader(shaderType);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    string shaderCode;
    ifstream shaderStream(file_path, ios::in);
    if (shaderStream.is_open())
    {
        string Line = "";
        while (getline(shaderStream, Line))
            shaderCode += "\n" + Line;
        shaderStream.close();
    }

    //Compile Shader
    printf("Compiling shader : %s\n", file_path);
    char const* sourcePointer = shaderCode.c_str();
    glShaderSource(ShaderID, 1, &sourcePointer, NULL);
    glCompileShader(ShaderID);

    //Check Shader
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> shaderErrorMessage( std::max(InfoLogLength, int(1)));
    glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &shaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &shaderErrorMessage[0]);

    return ShaderID;
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path,
    const char* tess_control_file_path, const char* tess_eval_file_path)
{
    //create the shaders
    GLuint VertexShaderID = CreateShader(GL_VERTEX_SHADER, vertex_file_path);
    GLuint FragmentShaderID = CreateShader(GL_FRAGMENT_SHADER, fragment_file_path);
    //GLuint GeometryShaderID = CreateShader(GL_GEOMETRY_SHADER, geometry_file_path);
    GLuint TessControlShaderID = CreateShader(GL_TESS_CONTROL_SHADER, tess_control_file_path);
    GLuint TessEvalShaderID = CreateShader(GL_TESS_EVALUATION_SHADER, tess_eval_file_path);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    //Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    //glAttachShader(ProgramID, GeometryShaderID);
    glAttachShader(ProgramID, TessControlShaderID);
    glAttachShader(ProgramID, TessEvalShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    //glDeleteShader(GeometryShaderID);
    glDeleteShader(TessControlShaderID);
    glDeleteShader(TessEvalShaderID);
 
    return ProgramID;
}

void init()
{
#ifdef WINDOWS
    //initilize the glew and check the errors.
    
    GLenum res = glewInit();
    if(res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
    }
#endif
    //select the background color
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    // Mouse, Keyboard input
    glutMouseFunc(mousePressed);
    glutKeyboardFunc(keyboardPressed);
}

void mousePressed(int btn, int state, int x, int y) 
{
    // Left click
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        
        cout << "mouse clicked" << endl;
        //if (isInputComplete) return;

        // Normalize clicked position to range[-1,1]
        vec2 normalizedPos = normalizedPosByLeftTop(x, y);
        
        // Add control point
        controlPoints.push_back(normalizedPos);
        cout << "Add control point" << endl;

        // show current control points
        for (unsigned int i = 0; i < controlPoints.size(); i++) {
            cout << "(" << controlPoints[i].x << " , " << controlPoints[i].y << ")" << endl;
        }
        cout << endl;

        // Transfer data from cpu to gpu
        if (controlPoints.size() >= 4) {
            isInputComplete = true;

            float* datas = new float[(controlPoints.size() + 1) * VERTEX_SIZE];
            datas[controlPoints.size() * VERTEX_SIZE] = '\0';
            datas[controlPoints.size() * VERTEX_SIZE + 1] = '\0';
            datas[controlPoints.size() * VERTEX_SIZE + 2] = '\0';

            for (unsigned int i = 0; i < controlPoints.size(); i++) {
                datas[3 * i] = controlPoints.at(i).x;
                datas[3 * i + 1] = controlPoints.at(i).y;
                datas[3 * i + 2] = 0;
            }

            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * controlPoints.size() * VERTEX_SIZE, datas, GL_STATIC_DRAW);
            delete[] datas;
            
            glutPostRedisplay();
        }
    }
}

void keyboardPressed(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);

    if (!isInputComplete) return;

    switch (key)
    {
        case 'w':
            cout << "w key pressed" << endl;
            tessel += TESSEL_DELATA;
            if (tessel > TESSEL_MAX) tessel = TESSEL_MAX;
            break;
        case 's':
            cout << "s key pressed" << endl;
            tessel -= TESSEL_DELATA;
            if (tessel < TESSEL_MIN) tessel = TESSEL_MIN;
            break;
        default:
            break;
    }
    
    GLuint tesselationID = glGetUniformLocation(programID, "u_tessellation");
    glUniform1i(tesselationID, tessel);
    cout << "Current Tessellation : " << tessel << endl << endl;

    glutPostRedisplay();
}

vec2 normalizedPosByLeftTop(int x, int y)
{
    vec2 result;
    result.x = (2.0f * x / SCREEN_WIDTH) - 1;
    result.y = 1 - (2.0f * y / SCREEN_HEIGHT);

    cout << "Normalize : (" << x << " , " << y << ") -> ("
                        << result.x << " , " << result.y << ")" << endl;
    return result;
}
