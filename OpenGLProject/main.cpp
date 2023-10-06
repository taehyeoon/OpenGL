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
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#endif
#include <GLut/glut.h>

using namespace std;
using namespace glm;

// Screen
const int SCREEN_WIDTH = 540;
const int SCREEN_HEIGHT = 540;

// Camera
const float CAMERA_SPEED = 0.05f;
float yaw = -90.0f;
float pitch = 0.0f;
vec3 cameraPos = vec3(0.0f ,0.0f ,3.0f);
vec3 cameraFront = vec3(0.0f ,0.0f ,-1.0f);
vec3 cameraUp = vec3(0.0f ,1.0f ,0.0f);

// Matrix
GLint mvpMatID;
mat4 modelMat = mat4(1.0f);
mat4 viewMat = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
mat4 projMat = perspective(radians(45.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 100.0f);
mat4 MVPMat = projMat * viewMat * modelMat;

// Mouse
float const MOUSE_SENSITIVITY = 0.1f;
bool isFirstRightClick = true;
bool isLeftClicking = false;
float lastMouseX;
float lastMouseY;

// Tessellation
GLuint tesselationID;
const int TESSEL_DELATA = 1;
const int TESSEL_MAX = 255;
const int TESSEL_MIN = 1;
int tessel = 10;

// Constant value
const double PI = 3.1415926;
const int VERTEX_SIZE = 3;
const int COLOR_SIZE = 4;
const int ONE_VERTEX_DATA_SIZE = VERTEX_SIZE + COLOR_SIZE;

// Program
bool isLineDrawingStart = false;
vector<vec2> controlPoints;
GLuint programID;

// Graphic
GLuint CreateShader(int shaderType, const char* file_path);
GLuint LoadShaders(const char* vertex_file_path,       bool isActiveV,
                   const char* tess_control_file_path, bool isActiveTC,
                   const char* tess_eval_file_path,    bool isActiveTE,
                   const char* geometry_file_path,     bool isActiveG,
                   const char* fragment_file_path,     bool isActiveF);
void init();
void renderScene(void);

// Input
void mouseDragged(int x, int y);
void mousePressed(int btn, int state, int x, int y);
void keyboardPressed(unsigned char key, int x, int y);
vec2 normalizedPosByLeftTop(int x, int y);

int main(int argc, char **argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
#ifdef WINDOWS
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
    
    programID = LoadShaders("VertexShader.txt",     true,
                            "TessControlShader.txt",true,
                            "TessEvalShader.txt",   true,
                            "GeometryShader.txt",   false,
                            "FragmentShader.txt",   true);
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
    tesselationID = glGetUniformLocation(programID, "u_tessellation");
    
    // Matrix
    mvpMatID = glGetUniformLocation(programID, "u_MVPMat");
    
    glutDisplayFunc(renderScene);

    //enter GLUT event processing cycle
    glutMainLoop();

    glDeleteVertexArrays(1, &VAO);

    return 0;
}

void renderScene(void)
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT);
    
    viewMat = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    MVPMat = projMat * viewMat * modelMat;
    glUniformMatrix4fv(mvpMatID, 1, false, &MVPMat[0][0]);
    
    glUniform1i(tesselationID, tessel);
    
    if (isLineDrawingStart) {
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

GLuint LoadShaders(const char* vertex_file_path,       bool isActiveV,
                   const char* tess_control_file_path, bool isActiveTC,
                   const char* tess_eval_file_path,    bool isActiveTE,
                   const char* geometry_file_path,     bool isActiveG,
                   const char* fragment_file_path,     bool isActiveF)
{
    GLuint VertexShaderID = 0;
    GLuint TessControlShaderID = 0;
    GLuint TessEvalShaderID = 0;
    GLuint GeometryShaderID = 0;
    GLuint FragmentShaderID = 0;
    
    
    //create the shaders
    if(isActiveV)
        VertexShaderID = CreateShader(GL_VERTEX_SHADER, vertex_file_path);
    if(isActiveTC)
        TessControlShaderID = CreateShader(GL_TESS_CONTROL_SHADER, tess_control_file_path);
    if(isActiveTE)
        TessEvalShaderID = CreateShader(GL_TESS_EVALUATION_SHADER, tess_eval_file_path);
    if(isActiveG)
        GeometryShaderID = CreateShader(GL_GEOMETRY_SHADER, geometry_file_path);
    if(isActiveF)
        FragmentShaderID = CreateShader(GL_FRAGMENT_SHADER, fragment_file_path);
    GLint Result = GL_FALSE;
    int InfoLogLength;

    //Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    if(isActiveV)
        glAttachShader(ProgramID, VertexShaderID);
    if(isActiveF)
        glAttachShader(ProgramID, FragmentShaderID);
    if(isActiveG)
        glAttachShader(ProgramID, GeometryShaderID);
    if(isActiveTC)
        glAttachShader(ProgramID, TessControlShaderID);
    if(isActiveTE)
        glAttachShader(ProgramID, TessEvalShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    if(isActiveV)
        glDeleteShader(VertexShaderID);
    if(isActiveTC)
        glDeleteShader(TessControlShaderID);
    if(isActiveTE)
        glDeleteShader(TessEvalShaderID);
    if(isActiveG)
        glDeleteShader(GeometryShaderID);
    if(isActiveF)
        glDeleteShader(FragmentShaderID);
 
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
    glutMotionFunc(mouseDragged);
    glutMouseFunc(mousePressed);
    glutKeyboardFunc(keyboardPressed);
}

void mouseDragged(int x, int y)
{
    // Left-click prevents you from moving your view
    if(isLeftClicking) return;
     
    // Initialize the previous mouse pointer position to the current mouse position
    // to prevent sudden change of viewpoint
    // when moving the field of view for the first time
    if(isFirstRightClick){
        lastMouseX = x;
        lastMouseY = y;
        isFirstRightClick = false;
        return;
    }
 
    cout << "Mouse drag : " << x << ", " << y << endl;

    float xoffset = x - lastMouseX;
    float yoffset = lastMouseY - y;
    lastMouseX = x;
    lastMouseY = y;

    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
    pitch = 89.0f;
    if(pitch < -89.0f)
    pitch = -89.0f;

    vec3 front;
    front.x = cos(radians(yaw)) * cos(radians(pitch));
    front.y = sin(radians(pitch));
    front.z = sin(radians(yaw)) * cos(radians(pitch));
    cameraFront = normalize(front);

    cout << "pos : " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << endl;
    cout << "front : " << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << endl;
    cout << "up : " << cameraUp.x << ", " << cameraUp.y << ", " << cameraUp.z << endl;
    cout << endl;

    glutPostRedisplay();

}

void mousePressed(int btn, int state, int x, int y)
{
    // Right click up
    if(btn == GLUT_RIGHT_BUTTON && state == GLUT_UP){
        cout << "right click up" << endl;
        isFirstRightClick = true;
    }

    if(btn == GLUT_LEFT_BUTTON && state == GLUT_UP){
        cout << "left click up" << endl;
        isLeftClicking = false;
    }
    
    // Left click down
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        cout << "mouse clicked" << endl;

        // If left click is continue, you can't move camera view
        isLeftClicking = true;

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
            isLineDrawingStart = true;

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
    // Terminate program
    if (key == 27){
        cout << "exit" << endl;
        exit(0);
    }

    cout << "key pressed : ";

    /// w : move forward
    /// a : move left
    /// s : move backward
    /// d : move right
    ///
    /// q : move down
    /// e : move up
    ///
    /// i : increase tessellation level
    /// k : decrease tessellation level
    switch (key)
    {
        case 'w':
            cout << "w key" << endl;
            cameraPos += cameraFront * CAMERA_SPEED;
            break;

        case 'a':
            cout << "a key" << endl;
            cameraPos -= normalize(cross(cameraFront, cameraUp)) * CAMERA_SPEED;
            break;

        case 's':
            cout << "s key" << endl;
            cameraPos -= cameraFront * CAMERA_SPEED;
            break;

        case 'd':
            cout << "d key" << endl;
            cameraPos += normalize(cross(cameraFront, cameraUp)) * CAMERA_SPEED;
            break;

        case 'q':
            cout << "q key"<< endl;
            cameraPos -= normalize(cameraUp) * CAMERA_SPEED;
            break;
            
        case 'e':
            cout << "e key" << endl;
            cameraPos += normalize(cameraUp) * CAMERA_SPEED;
            break;

        case 'i':
            cout << "i key" << endl;
            tessel += TESSEL_DELATA;
            if (tessel > TESSEL_MAX) tessel = TESSEL_MAX;
            break;
            
        case 'k':
            cout << "k key" << endl;
            tessel -= TESSEL_DELATA;
            if (tessel < TESSEL_MIN) tessel = TESSEL_MIN;
            break;
            
        default:
            break;
    }
    
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
