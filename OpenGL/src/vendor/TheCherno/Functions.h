#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../structs.h"
#include <fstream>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();

#ifdef DEBUG
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif


void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
std::string ReadFile(const std::string& filepath);
unsigned int CompileShader(const std::string& source, unsigned int type);
unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);