#include <iostream>
#include "init.hh"
using namespace std;

int initialize_GLFW()
{
    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        cerr << "Failed to initialize GLFW" << endl;
        return 1;
    }

    return 0;
}
int initialize_window(GLFWwindow* & window,int width,int height)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( width, height, "Shader Viewer", NULL, NULL);
    if( window == NULL ){
        cerr << "Failed to open GLFW window." << endl;
        return 1;
    }

    return 0; 
}


int initialize_GLEW(GLFWwindow* window)
{
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return 1;
    }

    return 0;
}

void initialize_keys(GLFWwindow* window)
{
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}
