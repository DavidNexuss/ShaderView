#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GLuint LoadShaders(const char * fragment_file_path);
GLuint loadTexture(const char * texture_file_path);
