#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include <fstream>
#include <string>
#include <sstream>

//__debugbreak()는 MSVC에만 사용 가능
#define ASSERT(x) if ((!(x))) __debugbreak(); 
#define GLCall(x) GLClearError();\
 				  x;\
 				  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragSource;
};

int legacyTriangles();

int modernTriangles();

// Shader
unsigned int compileShader(unsigned int type, const std::string& source);

unsigned int createShader(const std::string& vertexShader, const std::string& fragShader);

ShaderProgramSource parseShader(const std::string& filepath);

// Logger
void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);