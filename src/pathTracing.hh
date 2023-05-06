#pragma once
#include "window.hh"
#include "mesh.hh"

int pathTracingLoop(ShaderWindow &shaderwindow, const TMesh &screenMesh, GLuint programID);
void pathTracingInit(int argc, char** argv);
