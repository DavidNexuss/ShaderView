#include "profile.hh"

void Profile::loadUniforms(GLuint programID)
{
    iTime = glGetUniformLocation(programID,"iTime");
    iResolution = glGetUniformLocation(programID,"iResolution");
    iMouse = glGetUniformLocation(programID,"iMouse");
    iZoom = glGetUniformLocation(programID,"iZoom");
    iCameraTransform = glGetUniformLocation(programID,"iCameraTransform");
}

void Profile::flushUniforms()
{
    flushResolution();
    flushMouse();
    flushZoom();
    flushTime();
    flushCamera();
}