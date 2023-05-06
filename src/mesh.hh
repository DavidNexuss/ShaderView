#pragma once
#include "window.hh"

struct TMesh
{
    GLuint vao,vertexbuffer;

    inline void bind() const { glBindVertexArray(vao); }
    inline void draw() const
    {
        glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
    }

    static TMesh createScreenMesh();
};
