#include "pathTracing.hh"
#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <glm/gtx/transform.hpp>

extern "C" { 
#include <scene.h>
#include "../lib/cuda/scripts/test1.c"
}
#include "profile.hh"
#include "native.hh"


static GLuint cudaTexture;

Scene scene;
int pathTracingLoop(ShaderWindow &shaderwindow, const TMesh &screenMesh, GLuint programID) { 

  glUseProgram(programID);
  glBindTexture(GL_TEXTURE_2D, cudaTexture);
  GLFWwindow *window = shaderwindow.native();
  screenMesh.bind();
  GLuint loc = glGetUniformLocation(programID, "divisor");
  int index = 1;
  do {

    if(ProfileManager::currentProfile.move) { index = 0; }
    glUniform1f(loc, (float)(index + 1));
    SceneInput host = sceneInputHost(&scene);
    Camera& cam = host.constants->camera;
    glm::mat4 rotMatrix = glm::rotate(ProfileManager::currentProfile.mouse.x * 10.0f, glm::vec3(0,1,0)) * glm::rotate(ProfileManager::currentProfile.mouse.y * 10.0f, glm::vec3(1,0,0));

    cam.crossed   = make_float3(rotMatrix[0][0], rotMatrix[0][1], rotMatrix[0][2]);
    cam.up        = make_float3(rotMatrix[1][0], rotMatrix[1][1], rotMatrix[1][2]);
    cam.direction = make_float3(rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2]);
    glm::vec4 dir = rotMatrix * glm::vec4(ProfileManager::currentProfile.pos.x, ProfileManager::currentProfile.pos.y, ProfileManager::currentProfile.pos.z,1.0);
    dir *= 5.0f;
    dir.y *= -1.0f;
    cam.origin = make_float3(cam.origin.x + dir.x, cam.origin.y + dir.y, cam.origin.z + dir.z);
    host.constants->clear = index == 0;

    sceneUploadObjects(&scene);
    sceneRun(&scene);
    sceneDownload(&scene);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, scene.desc.frameBufferWidth, scene.desc.frameBufferHeight, GL_RGB, GL_FLOAT, sceneGetFrame(&scene, 0));
    index++;

    ProfileManager::currentProfile.loadUniforms(programID);
    ProfileManager::currentProfile.setViewport();
    ProfileManager::currentProfile.flushUniforms();

    screenMesh.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();

    if(ProfileManager::currentProfile.reset) { index = 0;}
  }

  while (glfwWindowShouldClose(window) == 0);

  return 0;
}

void pathTracingInit(int argc, char** argv) { 
  glGenTextures(1, &cudaTexture);
  glBindTexture(GL_TEXTURE_2D, cudaTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  SceneDesc sceneDesc = defaultDesc();
  sceneDesc.frameBufferWidth = 1280;
  sceneDesc.frameBufferHeight = 720;
  sceneDesc.numThreads = 1;
  sceneDesc.iterationsPerThread = 1;
  scene = sceneCreate(sceneDesc);
  traceInit(&scene);
  sceneUpload(&scene);

  SceneInput inp = sceneInputHost(&scene);
  traceLoop(inp.constants);

  sceneUploadObjects(&scene);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, sceneDesc.frameBufferWidth, sceneDesc.frameBufferHeight, 0, GL_RGB, GL_FLOAT, sceneGetFrame(&scene,0));
}
