#include "debug.hh"
#include <iostream>

void Debug::glError(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar *message,
        const void *userParam)
{
   std::cerr << message << std::endl;
}

void Debug::initDebug()
{
    glDebugMessageCallback(&Debug::glError,NULL);
    glEnable(GL_DEBUG_OUTPUT);
}