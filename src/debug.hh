#pragma once
#include <iostream>
#include <init.hh>

class Debug
{
    public:

    static void glError(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam);

    static void initDebug();
};