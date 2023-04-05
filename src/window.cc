#include "window.hh"
#include "util.hh"
#include "native.hh"
#include <iostream>

ShaderWindow::ShaderWindow(const ShaderWindowDescriptor& desc) : descriptor(desc) 
{
    shouldRecreate = 0;
}

void ShaderWindow::destroy(GLFWwindow*& w)
{
    if(w != nullptr)
    {
        glfwDestroyWindow(w);
        w = nullptr;
    }
}
ShaderWindow::ShaderWindow() : ShaderWindow({"name",800,600}) { }

ShaderWindow::~ShaderWindow()
{
    destroy(window);
}

void ShaderWindow::toggleBackgroundMode()
{
    descriptor.background = !descriptor.background;
    shouldRecreate = true;
}
void ShaderWindow::create()
{
    GLFWwindow* oldWindow = window;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    if(descriptor.no_decoration_flag)
    {
        glfwWindowHint(GLFW_DECORATED,false);
    }
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow( descriptor.width, descriptor.height, descriptor.name.c_str(), NULL, window);
    glfwWindowHint(GLFW_RESIZABLE,false);

    if( window == nullptr){
        std::cerr << "Failed to open GLFW window." << std::endl;
    }

    initialize_GLEW(window);
    if (descriptor.background)
    {
        //backgroundWindow(window);
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetWindowSizeCallback(window,descriptor.windowSizeCallback);
    glfwSetCursorPosCallback(window,descriptor.cursorPosCallback);
    glfwSetScrollCallback(window, descriptor.scrollCallback);
    glfwSetKeyCallback(window, descriptor.keyCallback);

    descriptor.windowSizeCallback(window,descriptor.width,descriptor.height);
    
    destroy(oldWindow);
}
