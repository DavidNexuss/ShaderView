#pragma once
#include "util.hh"

int initialize_GLFW();
int initialize_window(GLFWwindow* & window,int width,int height);
int initialize_GLEW(GLFWwindow* window);
void initialize_keys(GLFWwindow* window);
