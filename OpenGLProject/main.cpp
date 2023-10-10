#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cmath>
#include <random>
#include "stb_image.h"

#ifdef WINDOWS
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#endif
#include <GLut/glut.h>





using namespace std;
using namespace glm;

// Screen
const int SCREEN_WIDTH = 540;
const int SCREEN_HEIGHT = 540;

// Camera
const float CAMERA_SPEED = 0.05f;
float yawValue = -90.0f;
float pitchValue = 0.0f;
vec3 cameraPos = vec3(0.0f ,0.0f ,3.0f);
vec3 cameraFront = vec3(0.0f ,0.0f ,-1.0f);
vec3 cameraUp = vec3(0.0f ,1.0f ,0.0f);

// Matrix
GLint mvpMatID;
GLint modelMatID;
GLint viewMatID;
GLint projectionMatID;
mat4 modelMat = mat4(1.0f);
mat4 viewMat = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
mat4 projMat = perspective(radians(45.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.0001f, 100.0f);
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
const int FPS = 120;

// Program
bool isLineDrawingStart = false;
vector<vec2> controlPoints;
GLuint programID;

// Texture
GLuint texture1;
mat3 kernel = mat3(0,0,0,
                   0,1,0,
                   0,0,0);
float kernelSum = 1.0f;
bool isNegative = false;

// keyBoard
bool isWASDQE[6];

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
void mouseDown(int btn, int state, int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
vec2 normalizedPosByLeftTop(int x, int y);
void doMenu(int value);
void doTimer(int value);


int main(int argc, char **argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
#ifdef WINDOWS
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif
    
    //These two functions are used to define the position and size of the window.
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    //This is used to define the name of the window.
    glutCreateWindow("Simple OpenGL Window");
    
    //call initization function
    init();

    programID = LoadShaders("VertexShader.txt",     true,
                            "TessControlShader.txt",false,
                            "TessEvalShader.txt",   false,
                            "GeometryShader.txt",   false,
                            "FragmentShader.txt",   true);
    glUseProgram(programID);
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    
    float vertices[] = {
         // 위치               // 텍스처 좌표  // 노말 벡터
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // 좌측 상단 0 // 앞면
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 좌측 하단 1
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // 우측 상단 3
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 좌측 하단 1
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 우측 하단 2
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // 우측 상단 3
 
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f, // 우측 상단 3 // 우측
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 우측 하단 2
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f, // 우측 상단 7
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 우측 하단 2
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 우측 하단 6
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f, // 우측 상단 7
        
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 우측 상단 7 // 뒷면
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 우측 하단 6
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 좌측 상단 4
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 우측 하단 6
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 좌측 하단 5
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 좌측 상단 4
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 좌측 상단 4 // 죄측
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 좌측 하단 5
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 좌측 상단 0
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 좌측 하단 5
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 좌측 하단 1
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 좌측 상단 0
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 좌측 상단 4 // 윗면
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 좌측 상단 0
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 우측 상단 7
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 좌측 상단 0
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 우측 상단 3
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 우측 상단 7
        
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 우측 하단 6 // 아랫면
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 우측 하단 2
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 좌측 하단 5
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 우측 하단 2
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 좌측 하단 1
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 좌측 하단 5
    };

    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3,
                          GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture
    glVertexAttribPointer(1, 2,
                          GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal
    glVertexAttribPointer(2, 3,
                          GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Matrix
//    mvpMatID = glGetUniformLocation(programID, "u_MVPMat");
    modelMatID = glGetUniformLocation(programID, "u_modelMat");
    viewMatID = glGetUniformLocation(programID, "u_viewMat");
    projectionMatID = glGetUniformLocation(programID, "u_projectionMat");
    
    // Light color
    vec3 lightColor(1,0,1);
    glUniform3fv(glGetUniformLocation(programID, "u_lightColor"), 1, value_ptr(lightColor));
    
    // Light position
    vec3 lightPos(1,1,1);
    glUniform3fv(glGetUniformLocation(programID, "u_lightPos"), 1, value_ptr(lightPos));
    
    // Menu
    GLint subMenu = glutCreateMenu(doMenu);
    glutAddMenuEntry("cross", 3);
    glutAddMenuEntry("horizontal", 4);
    glutAddMenuEntry("vertical", 5);

    glutCreateMenu(doMenu);
    glutAddMenuEntry("identity", 0);
    glutAddMenuEntry("mean", 1);
    glutAddMenuEntry("sharpen", 2);
    glutAddMenuEntry("negative", -1);
    
    // Attach subMenu
    glutAddSubMenu("edge", subMenu);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);
    
    
    // Texture
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    // 현재 바인딩된 텍스쳐 객체에 대해 wrapping, filtering 옵션 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 텍스쳐 로드 및 생성
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    
    // Texture1
    unsigned char* data = stbi_load("lenna.png", &width, &height, &nrChannels, 0);
    if(data){
        // 현재 바인딩된 텍스쳐 객체가 첨부된 텍스쳐 이미지를 가지게 됨
        glTexImage2D(GL_TEXTURE_2D,             // 텍스쳐 타겟, GL_TEXURE_2D로 바인딩된 텍스쳐 객체에 텍스쳐를 생성한다는 의미
                     0,                         // 생성하는 텍스쳐의 mipmap 레벨을 수동으로 지정하고 싶을 때 지정, 베이스 레벨은 0
                     GL_RGBA,                    // 저장하고 싶은 텍스쳐가 가져야할 포멧 정보 전달, 여기서는 RGB값 정보만 가지고 있음
                     width, height,             // 텍스쳐의 너비와 높이
                     0,                         // boarder : 항상 0
                     GL_RGB, GL_UNSIGNED_BYTE,  // 원본 이미지의 포멧과 데이터 타입
                     data);                     // 실제 데이터
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        cout << "Failed to load texture" << endl;
    }
    stbi_image_free(data);
    
    // Set uniform attribute id in fragment shader
    glUniform1i(glGetUniformLocation(programID, "texture1"), 0);
    
    // Timer
    glutTimerFunc(1000.0f / FPS, doTimer, 1);
    
    // Render
    glutDisplayFunc(renderScene);

    //enter GLUT event processing cycle
    glutMainLoop();

    glDeleteVertexArrays(1, &VAO);

    return 0;
}

void renderScene(void)
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // MVP Matrix
    viewMat = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    MVPMat = projMat * viewMat * modelMat;
    glUniformMatrix4fv(modelMatID, 1, false, value_ptr(modelMat));
    glUniformMatrix4fv(viewMatID, 1, false, value_ptr(viewMat));
    glUniformMatrix4fv(projectionMatID, 1, false, value_ptr(projMat));
    
    // Transfer camera position to fragment shader for calculate specular light
    glUniform3fv(glGetUniformLocation(programID, "u_viewPos"), 1, value_ptr(cameraPos));
    
    // Texture kernel data
    glUniformMatrix3fv(glGetUniformLocation(programID, "u_kernel"), 1, false, value_ptr(kernel));
    glUniform1f(glGetUniformLocation(programID, "u_kernel_sum"), kernelSum);
    
    // Texture negative data
    glUniform1f(glGetUniformLocation(programID, "u_isNegative"), isNegative);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    
    modelMat = mat4(1.0);
    glUniformMatrix4fv(modelMatID, 1, false, value_ptr(modelMat));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    modelMat = translate(mat4(1.0), vec3(2,0,0));
    glUniformMatrix4fv(modelMatID, 1, false, value_ptr(modelMat));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    modelMat = translate(mat4(1.0), vec3(4,0,0));
    glUniformMatrix4fv(modelMatID, 1, false, value_ptr(modelMat));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    modelMat = translate(mat4(1.0), vec3(6,0,0));
    glUniformMatrix4fv(modelMatID, 1, false, value_ptr(modelMat));
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    // Mouse, Keyboard input
    glutMotionFunc(mouseDragged);
    glutMouseFunc(mouseDown);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
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
 
    float xoffset = x - lastMouseX;
    float yoffset = lastMouseY - y;
    lastMouseX = x;
    lastMouseY = y;

    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    yawValue += xoffset;
    pitchValue += yoffset;

    if(pitchValue > 89.0f)
        pitchValue = 89.0f;
    if(pitchValue < -89.0f)
        pitchValue = -89.0f;

    vec3 front;
    front.x = cos(radians(yawValue)) * cos(radians(pitchValue));
    front.y = sin(radians(pitchValue));
    front.z = sin(radians(yawValue)) * cos(radians(pitchValue));
    cameraFront = normalize(front);
}

void mouseDown(int btn, int state, int x, int y)
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
        cout << "left click down" << endl;

        // If left click is continue, you can't move camera view
        isLeftClicking = true;
    }
}

void keyboardDown(unsigned char key, int x, int y)
{
    // Terminate program
    if (key == 27){
        cout << "exit" << endl;
        exit(0);
    }

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
        case 'w': isWASDQE[0] = true; break;
        case 'a': isWASDQE[1] = true; break;
        case 's': isWASDQE[2] = true; break;
        case 'd': isWASDQE[3] = true; break;
        case 'q': isWASDQE[4] = true; break;
        case 'e': isWASDQE[5] = true; break;
        case 'i':
            cout << "i key" << endl;
            tessel += TESSEL_DELATA;
            if (tessel > TESSEL_MAX) tessel = TESSEL_MAX;
            cout << "Current Tessellation : " << tessel << endl << endl;
            break;
            
        case 'k':
            cout << "k key" << endl;
            tessel -= TESSEL_DELATA;
            if (tessel < TESSEL_MIN) tessel = TESSEL_MIN;
            cout << "Current Tessellation : " << tessel << endl << endl;
            break;
            
        default:
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch (key) {
        case 'w': isWASDQE[0] = false; break;
        case 'a': isWASDQE[1] = false; break;
        case 's': isWASDQE[2] = false; break;
        case 'd': isWASDQE[3] = false; break;
        case 'q': isWASDQE[4] = false; break;
        case 'e': isWASDQE[5] = false; break;
            
        default:
            break;
    }
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

void doMenu(int value){
    switch (value) {
        case 0:
            // Identity
            kernel = mat3(0,0,0,
                          0,1,0,
                          0,0,0);
            break;
        
            
        case 1:
            // Mean
            kernel = mat3(1,1,1,
                          1,1,1,
                          1,1,1);
            break;
            
        case 2:
            // Sharpen
            kernel = mat3(0,-1,0,
                          -1,5,-1,
                          0,-1,0);
            break;

        case 3:
            // Cross
            kernel = mat3(-1,-1,-1,
                          -1,8,-1,
                          -1,-1,-1);
            break;

        case 4:
            // Horizontal
            kernel = mat3(1,0,-1,
                          2,0,-2,
                          1,0,-1);
            break;

        case 5:
            // Vertical
            kernel = mat3(1,2,1,
                          0,0,0,
                          -1,-2,-1);
            break;

        case -1:
            isNegative = !isNegative;
            
        default:
            break;
    }
    
    kernelSum = 0;
    for(int i = 0; i< 3; i++){
        for(int j = 0; j<3; j++){
            kernelSum += kernel[i][j];
        }
    }
    
    // Prevent kernel sum is zero
    kernelSum = (kernelSum <= 0) ? 1 : kernelSum;
    
    glutPostRedisplay();
}

void doTimer(int value){
    
    if(isWASDQE[0]) cameraPos += cameraFront * CAMERA_SPEED; // w
    if(isWASDQE[1]) cameraPos -= normalize(cross(cameraFront, cameraUp)) * CAMERA_SPEED; // a
    if(isWASDQE[2]) cameraPos -= cameraFront * CAMERA_SPEED; // s
    if(isWASDQE[3]) cameraPos += normalize(cross(cameraFront, cameraUp)) * CAMERA_SPEED; // d
    if(isWASDQE[4]) cameraPos -= normalize(cameraUp) * CAMERA_SPEED; // q
    if(isWASDQE[5]) cameraPos += normalize(cameraUp) * CAMERA_SPEED; // e
    
    glutPostRedisplay();
    glutTimerFunc(1000.0/FPS, doTimer, 1);
}
