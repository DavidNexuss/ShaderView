#pragma once
#include <init.hh>
#include <glm/glm.hpp>

struct Profile 
{
    GLuint iResolution;
    GLuint iTime;
    GLuint iMouse;
    GLuint iZoom;
    GLuint iCameraTransform;

    glm::vec3 pos;
    glm::vec2 resolution;
    glm::vec2 mouse;
    glm::mat4 cameraTransform = glm::mat4(1.0f);
    float time;
    bool reset;
    float zoom;
    bool move;


    void loadUniforms(GLuint program);
    void flushUniforms();

    inline void flushResolution() { glUniform2fv(iResolution,1,&resolution[0]); }
    inline void flushTime() { glUniform1f(iTime,time); }
    inline void flushMouse() { glUniform2fv(iMouse,1,&mouse[0]); };
    inline void flushZoom() { glUniform1f(iZoom,zoom);};
    inline void flushCamera() { glUniformMatrix4fv(iCameraTransform,1,false,&cameraTransform[0][0]);};

    inline void setViewport() { glViewport(0,0,resolution.x,resolution.y); }

};

namespace ProfileManager
{
    inline Profile currentProfile;

    void loadProfile(const char* path);
    void saveProfile(const char* path);
};
