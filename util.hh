#pragma once
#define GLFW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint LoadShaders(const char * fragment_file_path,char* &errorBuffer);
GLuint loadTexture(const char * texture_file_path);
