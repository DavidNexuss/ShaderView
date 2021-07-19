#pragma once
#include "window.hh"

struct Mesh
{
    GLuint vao,vertexbuffer;

    inline void bind() const { glBindVertexArray(vao); }
    inline void draw() const
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
               0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               (void*)0            // array buffer offset
        );
        glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
    }

    static Mesh createScreenMesh();
};