#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int legacyTriangles();

int modernTriangles();

unsigned int compileShader(unsigned int type, const std::string& source);

unsigned int createShader(const std::string& vertexShader, const std::string& fragShader);
