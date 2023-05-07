#include "pathTracing.hh"
#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <glm/gtx/transform.hpp>

extern "C" { 
#include <scene.h>
#include <examples/examples.h>
}
#include "profile.hh"
#include "native.hh"


static GLuint cudaTexture;

SceneDesc defaultDesc() {

  SceneDesc sceneDesc;
  sceneDesc.maxMeshes           = 300;
  sceneDesc.maxObjects          = 400;
  sceneDesc.maxMaterials        = 300;
  sceneDesc.maxTextures         = 10;
  sceneDesc.maxVertexBuffer     = 10;
  sceneDesc.maxIndexBuffer      = 10;
  sceneDesc.frameBufferWidth    = 1920 / 2;
  sceneDesc.frameBufferHeight   = 1080 / 2;
  sceneDesc.numThreads          = 32;
  sceneDesc.iterationsPerThread = 1;
  sceneDesc.rayDepth            = 4;
  sceneDesc.framesInFlight      = 1;
  sceneDesc.frameDelta          = 0.1;
  sceneDesc.fWriteClamped       = 1;
  return sceneDesc;
}


Scene scene;
int pathTracingLoop(ShaderWindow &shaderwindow, const TMesh &screenMesh, GLuint programID) { 

  glUseProgram(programID);
  glBindTexture(GL_TEXTURE_2D, cudaTexture);
  GLFWwindow *window = shaderwindow.native();
  screenMesh.bind();
  GLuint loc = glGetUniformLocation(programID, "divisor");
  int index = 1;
  do {

    glUniform1f(loc, (float)(index + 1));
    SceneInput host = sceneInputHost(&scene);
    Camera& cam = host.constants->camera;
    glm::mat4 rotMatrix = glm::rotate(ProfileManager::currentProfile.mouse.x * 10.0f, glm::vec3(0,1,0)) * glm::rotate(ProfileManager::currentProfile.mouse.y * 10.0f, glm::vec3(1,0,0));

    cam.crossed   = make_float3(rotMatrix[0][0], rotMatrix[0][1], rotMatrix[0][2]);
    cam.up        = make_float3(rotMatrix[1][0], rotMatrix[1][1], rotMatrix[1][2]);
    cam.direction = make_float3(rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2]);
    cam.origin = make_float3(ProfileManager::currentProfile.pos.x, ProfileManager::currentProfile.pos.y, ProfileManager::currentProfile.pos.z);
    host.constants->clear = index == 0;

    sceneUploadObjects(&scene);
    sceneRun(&scene);
    sceneDownload(&scene);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, scene.desc.frameBufferWidth, scene.desc.frameBufferHeight, GL_RGB, GL_FLOAT, sceneGetFrame(&scene, 0));
    ProfileManager::currentProfile.loadUniforms(programID);
    ProfileManager::currentProfile.setViewport();
    ProfileManager::currentProfile.flushUniforms();

    screenMesh.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
    index++;

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
  scene = sceneCreate(sceneDesc);
  scene2(&scene);
  sceneUpload(&scene);

  SceneInput inp = sceneInputHost(&scene);
  scene2Loop(inp.constants);

  sceneUploadObjects(&scene);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, sceneDesc.frameBufferWidth, sceneDesc.frameBufferHeight, 0, GL_RGB, GL_FLOAT, sceneGetFrame(&scene,0));
}
